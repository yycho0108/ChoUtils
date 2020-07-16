#include "cho_util/vis/vtk_viewer.hpp"

#include <fmt/format.h>
#include <fmt/printf.h>

#include <chrono>
#include <thread>
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

#include "cho_util/core/thread_safe_queue.hpp"
#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/handlers.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

class VtkViewer::Impl {
  using ListenerPtr = cho::vis::ListenerPtr<RenderData>;
  using WriterPtr = cho::vis::WriterPtr;

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

  // Hmmm....
  thread_safe_queue<RenderData> render_queue_;

 public:
  Impl(const ListenerPtr listener, const WriterPtr writer, const bool start,
       const bool block = true);

 public:
  std::vector<int> GetAvailableHandlers(const int size) const;

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
  int RegisterHandler(Handler&& handler, const int rtype);

  int RegisterHandler(const Handler& handler, const int rtype);

  void RegisterDefaultHandlers();

  bool OnData(RenderData&& data);

  void Start(const bool block = true);

  void Step();

  void Spin();
};

// Forwarding calls to VtkViewer::Impl.
VtkViewer::VtkViewer(const ListenerPtr listener, const WriterPtr writer,
                     const bool start, const bool block)
    : impl_{new Impl(listener, writer, start, block)} {}
std::vector<int> VtkViewer::GetAvailableHandlers(const int size) const {
  return impl_->GetAvailableHandlers(size);
}
VtkViewer::~VtkViewer() = default;

int VtkViewer::RegisterHandler(Handler&& handler, const int rtype) {
  return impl_->RegisterHandler(std::move(handler), rtype);
}

int VtkViewer::RegisterHandler(const Handler& handler, const int rtype) {
  return impl_->RegisterHandler(handler, rtype);
}

void VtkViewer::RegisterDefaultHandlers() {
  return impl_->RegisterDefaultHandlers();
}

bool VtkViewer::OnData(RenderData&& data) {
  return impl_->OnData(std::move(data));
}

void VtkViewer::Start(const bool block) { return impl_->Start(block); }

void VtkViewer::Step() { return impl_->Step(); }

void VtkViewer::Spin() { return impl_->Spin(); }

VtkViewer::Impl::Impl(const ListenerPtr listener, const WriterPtr writer,
                      const bool start, const bool block)
    : data_listener_(listener), event_writer_(writer) {
  if (start) {
    Start(block);
  }
}

std::vector<int> VtkViewer::Impl::GetAvailableHandlers(const int size) const {
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
int VtkViewer::Impl::RegisterHandler(Handler&& handler, const int rtype) {
  return RegisterHandlerImpl(handler, &handlers_, rtype);
}

int VtkViewer::Impl::RegisterHandler(const Handler& handler, const int rtype) {
  return RegisterHandlerImpl(handler, &handlers_, rtype);
}

void VtkViewer::Impl::RegisterDefaultHandlers() {
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
              }},
      GetAvailableHandlers(1)[0]);
}

bool VtkViewer::Impl::OnData(RenderData&& data) {
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
    auto actor = cho::vis::Render(colors, data);
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
      cho::vis::Update(amap[data.tag], data);
    }
  }

  // Invalidate render cache.
  should_render = true;
  return false;
}

void VtkViewer::Impl::Start(const bool block) {
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
  render_window_interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  render_window_interactor->SetRenderWindow(render_window);
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  render_window_interactor->SetInteractorStyle(style);

  // ListenerPtr on adding new primitives.
  // Register callback for updating or creating actors based on pipe input.
  // Start feeding data.

  data_listener_->SetCallback([this](RenderData&& data) -> bool {
    // return OnData(std::move(data));
    render_queue_.emplace(std::move(data));
    return false;
  });

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
        fmt::print("Timer\n");
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

void VtkViewer::Impl::Step() {
  render_window_interactor->ProcessEvents();
  if (!render_queue_.empty()) {
    OnData(std::move(render_queue_.front()));
    render_queue_.pop();
    render_window->Render();
  }
}

void VtkViewer::Impl::Spin() {
  // render_window_interactor->Start();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    render_window_interactor->ProcessEvents();
    if (!render_queue_.empty()) {
      OnData(std::move(render_queue_.front()));
      render_queue_.pop();
      render_window->Render();
    }
  }
}
}  // namespace vis
}  // namespace cho
