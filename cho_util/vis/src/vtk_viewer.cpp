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

using HandlerVariant =
    std::variant<CloudHandler, CylinderHandler, CuboidHandler, SphereHandler,
                 PlaneHandler, LineHandler>;

class VtkViewer::Impl {
  using ListenerPtr = cho::vis::ListenerPtr<RenderData>;
  using WriterPtr = cho::vis::WriterPtr;

 private:
  // I/O
  ListenerPtr data_listener_;
  WriterPtr event_writer_;

  // Cached render resources.
  std::unordered_map<std::string, RenderData> rmap;
  // std::unordered_map<std::string, vtkSmartPointer<vtkActor>> amap;

  std::unordered_map<std::string, HandlerVariant> hmap;

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
  bool OnData(RenderData&& data);

  void Start(const bool block = true);

  void Step();

  void Spin();
};

// Forwarding calls to VtkViewer::Impl.
VtkViewer::VtkViewer(const ListenerPtr listener, const WriterPtr writer,
                     const bool start, const bool block)
    : impl_{new Impl(listener, writer, start, block)} {}

VtkViewer::~VtkViewer() = default;

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

bool VtkViewer::Impl::OnData(RenderData&& data) {
  vtkSmartPointer<vtkNamedColors> colors =
      vtkSmartPointer<vtkNamedColors>::New();

  // Early exit and skip processing.
  if (data.quit) {
    // TODO(yycho0108): Signal quit status externally as well.
    std::flush(std::cout);
    return data.quit;
  }

  // Copy or move data to rmap to avoid data from going out of scope.
  // This way, all primitives will have valid data for this duration.
  const std::string tag = data.tag;
  auto it = rmap.find(tag);
  const bool create = (it == rmap.end());
  if (create) {
    rmap.emplace(tag, std::forward<RenderData>(data));
  } else {
    it->second = std::forward<RenderData>(data);
  }

  if (create) {
    // Create
    const auto& data = rmap[tag];

    std::visit(
        cho::vis::overloaded{[this, &data](const core::Cuboid<float, 3>&) {
                               auto h = CuboidHandler{data};
                               renderer->AddActor(h.GetActor());
                               hmap.emplace(data.tag, h);
                             },
                             [this, &data](const core::Cylinder<float>&) {
                               auto h = CylinderHandler{data};
                               renderer->AddActor(h.GetActor());
                               hmap.emplace(data.tag, h);
                             },
                             [this, &data](const core::Line<float, 3>&) {
                               // auto h = LineHandler{data};
                               // renderer->AddActor(h.GetActor());
                               // hmap.emplace(data.tag, h);
                             },
                             [this, &data](const core::Sphere<float, 3>&) {
                               auto h = SphereHandler{data};
                               renderer->AddActor(h.GetActor());
                               hmap.emplace(data.tag, h);
                             },

                             [this, &data](const core::PointCloud<float, 3>&) {
                               auto h = CloudHandler{data};
                               renderer->AddActor(h.GetActor());
                               hmap.emplace(data.tag, h);
                             },

                             [this, &data](const core::Plane<float, 3>&) {
                               auto h = PlaneHandler{data};
                               renderer->AddActor(h.GetActor());
                               hmap.emplace(data.tag, h);
                             },
                             [this, &data](const core::Lines<float, 3>&) {
                               auto h = LineHandler{data};
                               renderer->AddActor(h.GetActor());
                               hmap.emplace(data.tag, h);
                             },
                             [](const auto&) {

                             }},
        data.geometry);
  } else {
    // Update
    const auto& data = rmap[tag];
    std::visit([&data](auto& h) { h.Update(data); }, hmap.at(data.tag));
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
