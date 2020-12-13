#include "cho_util/vis/imgui_viewer/imgui_viewer.hpp"

#include <fmt/printf.h>

// Imgui dep
#include <glad/glad.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

// Include glfw3.h after IMGUI OpenGL definitions
#include <GLFW/glfw3.h>

#include "cho_util/core/thread_safe_queue.hpp"
#include "cho_util/util/mp_utils.hpp"
#include "cho_util/vis/event_data.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace vis {

class ImguiViewer::Impl {
 public:
  using ListenerPtr = cho::io::ListenerPtr<RenderData>;
  using WriterPtr = cho::io::WriterPtr;
  explicit Impl(const ListenerPtr listener, const WriterPtr writer,
                const bool start, const bool block = true);
  ~Impl();

 public:
  void Start(const bool block = true);
  void Step();
  void Spin();
  bool Render(RenderData&& data);

 private:
  bool TrySetup();
  // I/O
  ListenerPtr data_listener_;
  WriterPtr event_writer_;

  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  GLFWwindow* window_;

  //// Cached render resources.
  // std::unordered_map<std::string, RenderData> rmap_;
  // std::unordered_map<std::string, HandlerVariant> hmap_;

  //// Application state.
  // bool should_render_{true};

  //// Hmmm....
  // thread_safe_queue<RenderData> render_queue_;
};

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

ImguiViewer::Impl::Impl(const ListenerPtr listener, const WriterPtr writer,
                        const bool start, const bool block)
    : data_listener_(listener), event_writer_(writer) {
  if (start) {
    Start(block);
  }
}

ImguiViewer::Impl::~Impl() {
  // Cleanup - maybe should be included in Stop()?
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

void ImguiViewer::Impl::Start(const bool block) {
  if (!TrySetup()) {
    fmt::print(std::cerr, "Setup failed: aborting.\n");
    return;
  }
  if (block) {
    Spin();
  }
}

bool ImguiViewer::Impl::TrySetup() {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    fmt::print(std::cerr, "glfwinit failed\n");
    return false;
  }

  // Decide GL+GLSL versions
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only

  // Create window with graphics context
  window_ = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL,
                             NULL);
  if (window_ == nullptr) {
    fmt::print(std::cerr, "window null\n");
    return false;
  }
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);  // Enable vsync

  // Initialize OpenGL loader.
  bool err = gladLoadGL() == 0;
  if (err) {
    fmt::print(std::cerr, "Failed to initialize OpenGL loader!\n");
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad
  // Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport
                                                       // / Platform Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform
  // windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);
  return true;
}

bool ImguiViewer::Impl::Render(RenderData&& data) { return true; }

void ImguiViewer::Impl::Step() {
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell
  // if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
  // your main application.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data
  // to your main application. Generally you may always pass all inputs to dear
  // imgui, and hide them from your application based on those two flags.
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // 1. Show the big demo window (Most of the sample code is in
  // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
  // ImGui!).
  if (show_demo_window) {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair
  // to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!"
                                    // and append into it.

    ImGui::Text("This is some useful text.");  // Display some text (you can use
                                               // a format strings too)
    ImGui::Checkbox(
        "Demo Window",
        &show_demo_window);  // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3(
        "clear color",
        (float*)&clear_color);  // Edit 3 floats representing a color

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most
                                  // widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }

  // 3. Show another simple window.
  if (show_another_window) {
    ImGui::Begin(
        "Another Window",
        &show_another_window);  // Pass a pointer to our bool variable (the
                                // window will have a closing button that will
                                // clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) show_another_window = false;
    ImGui::End();
  }

  // Rendering
  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(window_, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  /*Here we add some code to add the 3d world*/
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  // (Platform functions may change the current OpenGL context, so we
  // save/restore it to make it easier to paste this code elsewhere.
  //  For this specific demo app we could also call
  //  glfwMakeContextCurrent(window) directly)
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
  glfwSwapBuffers(window_);
}

void ImguiViewer::Impl::Spin() {
  while (!glfwWindowShouldClose(window_)) {
    Step();
  }
}

// Forwarding calls to ImguiViewer::Impl.
ImguiViewer::ImguiViewer(const ListenerPtr listener, const WriterPtr writer,
                         const bool start, const bool block)
    : impl_{std::make_unique<Impl>(listener, writer, start, block)} {}

ImguiViewer::~ImguiViewer() = default;

bool ImguiViewer::Render(RenderData&& data) {
  return impl_->Render(std::move(data));
}

void ImguiViewer::Start(const bool block) { return impl_->Start(block); }

void ImguiViewer::Step() { return impl_->Step(); }

void ImguiViewer::Spin() { return impl_->Spin(); }

}  // namespace vis
}  // namespace cho
