#include "cho_util/vis/embedded_viewer.hpp"

#include <array>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkBoundingBox.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPlaneSource.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include "cho_util/vis/handlers.hpp"
#include "cho_util/vis/pipe_io.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/subprocess.hpp"

namespace cho_util {
namespace vis {

void EmbeddedViewer::Start() {
  if (started_) {
    throw std::runtime_error("Already started");
    return;
  }
  started_ = true;
  proc.Start();
  if (IsServer()) {
    RegisterDefaultHandlers();
    StartServer();
  } else {
    rc = std::make_shared<FdWriter>(proc.GetWriteFd());
  }
}

std::vector<int> EmbeddedViewer::GetAvailableHandlers(const int size) const {
  std::vector<int> out;
  out.reserve(size);

  int index = RenderData::kUser;
  while (out.size() < size) {
    if (handlers_.find(index) != handlers_.end()) {
      ++index;
      continue;
    }
    out.emplace_back(index);
  }
  return out;
}

template <typename HandlerRef>
int RegisterHandlerImpl(HandlerRef handler,
                        std::unordered_map<int, HandlerPtr>* const handlers,
                        const int rtype = -1) {
  int rtype_{-1};
  if (rtype >= RenderData::kUser) {
    // Use supplied id.
    if (handlers->find(rtype) != handlers->end()) {
      return -1;
    }
    rtype_ = rtype;
  } else {
    // Try to assign an unused handler id.
    while (true) {
      if (handlers->find(rtype_) == handlers->end()) {
        break;
        ++rtype_;
      }
    }
  }

  // Set + return handler id.
  handlers->emplace(rtype_, std::make_shared<Handler>(handler));
  return rtype_;
}

int EmbeddedViewer::RegisterHandler(Handler&& handler, const int rtype) {
  if (IsClient()) {
    return -1;
  }
  return RegisterHandlerImpl(handler, &handlers_, rtype);
}

int EmbeddedViewer::RegisterHandler(const Handler& handler, const int rtype) {
  if (IsClient()) {
    return -1;
  }
  return RegisterHandlerImpl(handler, &handlers_, rtype);
}

/**
 *
 *
 */
void EmbeddedViewer::RegisterDefaultHandlers() {
  vtkSmartPointer<vtkNamedColors> colors =
      vtkSmartPointer<vtkNamedColors>::New();

  // Example: custom plane
  RegisterHandler(
      Handler{[colors](const RenderData& rd) -> vtkSmartPointer<vtkActor> {
                return GetPlaneActor(colors, &rd.data[0], &rd.data[3]);
              },
              [](const vtkSmartPointer<vtkActor>& actor, const RenderData& rd) {
                vtkSmartPointer<vtkAlgorithm> alg =
                    actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
                vtkSmartPointer<vtkPlaneSource> plane =
                    vtkPlaneSource::SafeDownCast(alg);
                plane->SetCenter(rd.data[0], rd.data[1], rd.data[2]);
                plane->SetNormal(rd.data[3], rd.data[4], rd.data[5]);
                plane->Update();
              }});
}

void EmbeddedViewer::StartServer() {
  vtkSmartPointer<vtkNamedColors> colors =
      vtkSmartPointer<vtkNamedColors>::New();

  // Set the background color.
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // Zoom in a little by accessing the camera and invoking its "Zoom" method.
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(1.5);
  renderer->GetActiveCamera()->SetPosition(5,5,5);

  // The render window is the actual GUI window
  // that appears on the computer screen
  vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(768, 768);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("EmbeddedViewer");

  // Interaction
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = 
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  renderWindowInteractor->SetInteractorStyle( style );


  // Listener on adding new primitives.
  std::unordered_map<std::string, RenderData> rmap;
  std::unordered_map<std::string, vtkSmartPointer<vtkActor>> amap;

  bool should_render = false;

  // Register callback for updating or creating actors based on pipe input.
  std::function<bool(RenderData &&)> on_data = [&](RenderData&& data) -> bool {
    // Early exit and skip processing.
    if (data.quit) {
      // TODO(yycho0108): Signal quit status externally as well.
      std::flush(std::cout);
      return data.quit;
    }

    // Copy data to rmap to avoid data from going out of scope.
    // This way, all primitives will have valid data for this duration.
    const std::string tag = data.tag;
    auto it = rmap.find(tag);
    bool create = false;
    if (it != rmap.end()) {
      it->second = std::move(data);
    } else {
      rmap.emplace(tag, std::move(data));
      create = true;
    }

    should_render = true;

    if (create) {
      // Create
      const auto& data = rmap[tag];
      auto actor = cho_util::vis::Render(colors, data);
      if (!actor) {
        auto it = handlers_.find(data.render_type);
        if (it != handlers_.end()) {
          fmt::print("Fallback\n");
          actor = it->second->create(data);
        }
        if (!actor) {
          return false;
        }
      }
      renderer->AddActor(actor);
      amap[data.tag] = actor;
    } else {
      // Update
      const auto& data = rmap[tag];
      auto it = handlers_.find(data.render_type);
      if (it != handlers_.end()) {
        it->second->update(amap[data.tag], data);
      } else {
          cho_util::vis::Update(amap[data.tag], data);
      }
    }

    return false;
  };
  FdListener<RenderData> listener(proc.GetReadFd(), on_data);
  listener.Start();

  // Periodically refresh the window to reflect updated data.
  // Only render if necessary!
  struct ClientData {
    vtkSmartPointer<vtkRenderWindow>& renderWindow;
    bool& should_render;
  };
  ClientData cd{renderWindow, should_render};
  vtkSmartPointer<vtkCallbackCommand> onUpdate =
      vtkSmartPointer<vtkCallbackCommand>::New();
  onUpdate->SetClientData(&cd);
  onUpdate->SetCallback(
      [](vtkObject* o, long unsigned int, void* client_data, void*) {
        ClientData& cd = *static_cast<ClientData*>(client_data);
        if (cd.should_render) {
          cd.renderWindow->Render();
          cd.should_render = false;
        }
      });

  // Start event loop.
  renderWindow->Render();
  renderWindowInteractor->Initialize();
  renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, onUpdate);
  renderWindowInteractor->CreateRepeatingTimer(100);
  renderWindowInteractor->Start();

  // Always exit. Hopefully will be somewhat graceful
  std::exit(EXIT_SUCCESS);
}

void EmbeddedViewer::Render(const RenderData& data) {
  if (!rc) {
    return;
  }
  rc->Send<RenderData>(data);
}
}  // namespace vis
}  // namespace cho_util
