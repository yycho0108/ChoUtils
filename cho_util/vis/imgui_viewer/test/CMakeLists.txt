find_package(Threads REQUIRED)

add_executable(imgui_viewer_test imgui_viewer_test.cpp)
target_link_libraries(imgui_viewer_test PRIVATE cho_util_imgui_viewer
                                                Threads::Threads)
