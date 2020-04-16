#pragma once

#include "cho_util/vis/vtk_viewer_fwd.hpp"

#include <fmt/format.h>
#include <fmt/printf.h>

#include <unordered_map>
#include <vector>

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkBoundingBox.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPlaneSource.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>

#include "cho_util/vis/handlers.hpp"
#include "cho_util/vis/pipe_io.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho_util {
namespace vis {

template <typename ListenerPtr, typename WriterPtr>
class VtkViewer {
 private:
  // I/O
  ListenerPtr data_listener_;
  WriterPtr event_writer_;
  std::unordered_map<int, HandlerPtr> handlers_;

  // Cached render resources.
  std::unordered_map<std::string, RenderData> rmap;
  std::unordered_map<std::string, vtkSmartPointer<vtkActor>> amap;

  // Application state.
  vtkSmartPointer<vtkRenderWindow> render_window;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindowInteractor> render_window_interactor;
  bool should_render{true};

 public:
  VtkViewer(const ListenerPtr& listener, const WriterPtr& writer,
            const bool start, const bool block = true)
      : data_listener_(listener), event_writer_(writer) {
    if (start) {
      Start(block);
    }
  }

 public:
  std::vector<int> GetAvailableHandlers(const int size) const {
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

  int RegisterHandler(Handler&& handler, const int rtype) {
    return RegisterHandlerImpl(handler, &handlers_, rtype);
  }

  int RegisterHandler(const Handler& handler, const int rtype) {
    return RegisterHandlerImpl(handler, &handlers_, rtype);
  }

  /**
   *
   *
   */
  void RegisterDefaultHandlers() {
    vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();

    // Example: custom plane
    RegisterHandler(Handler{
        [colors](const RenderData& rd) -> vtkSmartPointer<vtkActor> {
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

  bool OnData(RenderData&& data) {
    vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();

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
    const bool create = (it == rmap.end());
    if (create) {
      rmap.emplace(tag, std::move(data));
    } else {
      it->second = std::move(data);
    }

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

    // Invalidate render cache.
    should_render = true;
    return false;
  }

  void Start(const bool block = true) {
    // Set the background color.
    vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();
    std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
    colors->SetColor("BkgColor", bkg.data());

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
    // Zoom in a little by accessing the camera and invoking its "Zoom" method.
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Zoom(1.5);
    // renderer->GetActiveCamera()->SetPosition(5, 5, 5);

    // The render window is the actual GUI window
    // that appears on the computer screen
    render_window = vtkSmartPointer<vtkRenderWindow>::New();
    render_window->SetSize(768, 768);
    render_window->AddRenderer(renderer);
    render_window->SetWindowName("VtkViewer");

    // Interaction
    render_window_interactor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    render_window_interactor->SetRenderWindow(render_window);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    render_window_interactor->SetInteractorStyle(style);

    // ListenerPtr on adding new primitives.
    // Register callback for updating or creating actors based on pipe input.
    // Start feeding data.
    data_listener_->SetCallback(
        [this](RenderData&& data) { return OnData(std::move(data)); });
    if (!data_listener_->IsRunning()) {
      data_listener_->Start();
    }

    // Periodically refresh the window to reflect updated data.
    // Only render if necessary!
    vtkSmartPointer<vtkCallbackCommand> onUpdate =
        vtkSmartPointer<vtkCallbackCommand>::New();
    onUpdate->SetClientData(this);
    onUpdate->SetCallback(
        [](vtkObject* o, long unsigned int, void* client_data, void*) {
          auto viewer = static_cast<decltype(this)>(client_data);
          if (viewer->should_render) {
            viewer->render_window->Render();
            viewer->should_render = false;
          }
        });

    // Start event loop.
    // TODO(yycho0108): Forward events produced from the viewer to
    // event_writer_.
    render_window->Render();
    render_window_interactor->Initialize();
    render_window_interactor->AddObserver(vtkCommand::TimerEvent, onUpdate);
    render_window_interactor->CreateRepeatingTimer(100);
    if (block) {
      Spin();
      // Always exit. Hopefully will be somewhat graceful
      std::exit(EXIT_SUCCESS);
    }
  }

  void Step() { render_window_interactor->ProcessEvents(); }
  void Spin() { render_window_interactor->Start(); }
};

}  // namespace vis
}  // namespace cho_util
