#include "cho_util/vis/imgui_viewer/imgui_viewer.hpp"

#include "cho_util/io/queue_io.hpp"
#include "cho_util/vis/render_data.hpp"

int main() {
  auto data_queue = std::make_shared<thread_safe_queue<cho::vis::RenderData>>();
  auto event_queue =
      std::make_shared<thread_safe_queue<cho::vis::RenderData>>();
  auto event_writer =
      std::make_shared<cho::io::QueueWriter<cho::vis::RenderData>>(event_queue);
  auto data_listener =
      std::make_shared<cho::io::QueueListener<cho::vis::RenderData>>(
          data_queue);
  auto viewer = std::make_shared<cho::vis::ImguiViewer>(
      data_listener, event_writer, true, true);
}
