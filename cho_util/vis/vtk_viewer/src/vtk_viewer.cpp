#include "cho_util/vis/vtk_viewer/vtk_viewer.hpp"

#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>
#include <fmt/printf.h>

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "cho_util/core/thread_safe_queue.hpp"
#include "cho_util/util/mp_utils.hpp"
#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/render_data.hpp"
#include "cho_util/vis/vtk_viewer/handlers.hpp"

namespace cho {
namespace vis {

class PickableTrackballCamera : public vtkInteractorStyleTrackballCamera {
 public:
  static PickableTrackballCamera* New();
  vtkTypeMacro(PickableTrackballCamera, vtkInteractorStyleTrackballCamera);

  virtual void OnLeftButtonDown() override {
    fmt::print("ONLMB\n");
    int* clickPos = this->GetInteractor()->GetEventPosition();

    // Pick from this location.
    vtkSmartPointer<vtkPropPicker> picker =
        vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

    double* pos = picker->GetPickPosition();
    std::cout << "Pick position (world coordinates) is: " << pos[0] << " "
              << pos[1] << " " << pos[2] << std::endl;
    std::cout << "Picked actor: " << picker->GetActor() << std::endl;
    // Forward events
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
  }
};
vtkStandardNewMacro(PickableTrackballCamera);

class VtkViewer::Impl {
 public:
  using ListenerPtr = cho::io::ListenerPtr<RenderData>;
  using WriterPtr = cho::io::WriterPtr;
  Impl(const ListenerPtr listener, const WriterPtr writer, const bool start,
       const bool block = true);

 public:
  void Start(const bool block = true);
  void Step();
  void Spin();
  bool Render(RenderData&& data);

  void OnPick(vtkObject* caller, long unsigned int event_id, void* client_data);

 private:
  // I/O
  ListenerPtr data_listener_;
  WriterPtr event_writer_;

  // Cached render resources.
  std::unordered_map<std::string, RenderData> rmap_;
  std::unordered_map<std::string, HandlerVariant> hmap_;

  // Application state.
  vtkSmartPointer<vtkRenderWindow> render_window_;
  vtkSmartPointer<vtkRenderer> renderer_;
  vtkSmartPointer<vtkRenderWindowInteractor> render_window_interactor_;
  bool should_render_{true};

  // Hmmm....
  thread_safe_queue<RenderData> render_queue_;
};

// Forwarding calls to VtkViewer::Impl.
VtkViewer::VtkViewer(const ListenerPtr listener, const WriterPtr writer,
                     const bool start, const bool block)
    : impl_{new Impl(listener, writer, start, block)} {}

VtkViewer::~VtkViewer() = default;

bool VtkViewer::Render(RenderData&& data) {
  return impl_->Render(std::move(data));
}

void VtkViewer::Start(const bool block) { return impl_->Start(block); }

void VtkViewer::Step() { return impl_->Step(); }

void VtkViewer::Spin() { return impl_->Spin(); }

VtkViewer::Impl::Impl(const ListenerPtr listener, const WriterPtr writer,
                      const bool start, const bool block)
    : data_listener_(listener), event_writer_(writer) {
  if (start) {
    fmt::print("Starting viewer, block = {}\n", block);
    Start(block);
  }
}

void VtkViewer::Impl::Start(const bool block) {
  // Set the background color.
  vtkSmartPointer<vtkNamedColors> colors =
      vtkSmartPointer<vtkNamedColors>::New();
  std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
  colors->SetColor("BkgColor", bkg.data());

  renderer_ = vtkSmartPointer<vtkRenderer>::New();
  renderer_->SetBackground(colors->GetColor3d("BkgColor").GetData());
  // Zoom in a little by accessing the camera and invoking its "Zoom" method.
  renderer_->ResetCamera();
  renderer_->GetActiveCamera()->Zoom(1.5);
  // renderer_->GetActiveCamera()->SetPosition(5, 5, 5);

  // The render window is the actual GUI window
  // that appears on the computer screen
  render_window_ = vtkSmartPointer<vtkRenderWindow>::New();
  render_window_->SetSize(768, 768);
  render_window_->AddRenderer(renderer_);
  render_window_->SetWindowName("VtkViewer");

  // Interaction
  render_window_interactor_ = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  render_window_interactor_->SetRenderWindow(render_window_);
  vtkSmartPointer<PickableTrackballCamera> style =
      vtkSmartPointer<PickableTrackballCamera>::New(/*this*/);
  style->SetDefaultRenderer(renderer_);
  render_window_interactor_->SetInteractorStyle(style);

  // render_window_interactor_->AddObserver(vtkCommand::KeyPressEvent, this,
  //                                       &VtkViewer::Impl::OnLeftClick);

  // ListenerPtr on adding new primitives.
  // Register callback for updating or creating actors based on pipe input.
  // Start feeding data.
  data_listener_->SetCallback([this](RenderData&& data) -> bool {
    // return Render(std::move(data));
    // RenderData cpy = data;
    // render_queue_.emplace(std::move(data));
    render_queue_.emplace(std::move(data));
    return false;
  });

  // Periodically refresh the window to reflect updated data.
  // Only render if necessary!
  vtkSmartPointer<vtkCallbackCommand> onUpdate =
      vtkSmartPointer<vtkCallbackCommand>::New();
  onUpdate->SetClientData(this);
  onUpdate->SetCallback(
      [](vtkObject* o, long unsigned int, void* client_data, void*) {
        fmt::print("Timer\n");
        auto* const viewer = static_cast<decltype(this)>(client_data);
        if (viewer->should_render_) {
          viewer->render_window_->Render();
          viewer->should_render_ = false;
        }
      });

  // Start event loop.
  // TODO(yycho0108): Forward events produced from the viewer to
  // event_writer_.
  render_window_->Render();
  render_window_interactor_->Initialize();
  render_window_interactor_->AddObserver(vtkCommand::TimerEvent, onUpdate);
  // render_window_interactor_->CreateRepeatingTimer(100);

  if (!data_listener_->IsRunning()) {
    data_listener_->Start();
  }
  fmt::print("End of initialization\n");
  if (block) {
    Spin();
    // Always exit. Hopefully will be somewhat graceful
    std::exit(EXIT_SUCCESS);
  }
}

void VtkViewer::Impl::Step() {
  render_window_interactor_->ProcessEvents();
  if (!render_queue_.empty()) {
    Render(std::move(render_queue_.front()));
    render_queue_.pop();
    render_window_->Render();
  }
}

void VtkViewer::Impl::Spin() {
  // render_window_interactor_->Start();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Step();
  }
}

void VtkViewer::Impl::OnPick(vtkObject* caller, unsigned long event_id,
                             void* client_data) {
  fmt::print("ONPICK\n");
}

bool VtkViewer::Impl::Render(RenderData&& data) {
  // Early exit and skip processing.
  if (data.quit) {
    // TODO(yycho0108): Signal quit status externally as well.
    std::flush(std::cout);
    return data.quit;
  }

  // Copy or move data to rmap_ to avoid data from going out of scope.
  // This way, all primitives will have valid data for this duration.
  const std::string tag = data.tag;
  auto it = rmap_.find(tag);
  const bool create = (it == rmap_.end());
  if (create) {
    rmap_.emplace(tag, std::move(data));
  } else {
    it->second = std::move(data);
  }

  // Instead of using data directly, use cached data.
  const auto& cache = rmap_[tag];
  if (create) {
    // Create
    std::visit(
        [this, &cache](const auto& geometry) {
          using T = std::decay_t<decltype(geometry)>;
          HandlerType<T> h{cache};
          renderer_->AddActor(h.GetActor());
          hmap_.emplace(cache.tag, std::move(h));
        },
        data.geometry);
  } else {
    // Update
    std::visit([&cache](auto& h) { h.Update(cache); }, hmap_.at(cache.tag));
  }

  // Invalidate render cache.
  should_render_ = true;
  return false;
}

}  // namespace vis
}  // namespace cho
