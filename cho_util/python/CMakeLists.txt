include(FetchContent)

# pybind11
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY git@github.com:pybind/pybind11.git
  GIT_TAG v2.6.2
  GIT_SHALLOW 1
  SOURCE_DIR
  pybind11)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(cho_util_python SHARED src/module.cpp)

target_link_libraries(cho_util_python PUBLIC ChoUtil::core ChoUtil::util
                                             ChoUtil::type ChoUtil::proto)

# OK, I'm not really sure if the below two stuff is needed.
add_library(${PROJECT_NAME}::python ALIAS cho_util_python)
set_target_properties(cho_util_python PROPERTIES EXPORT_NAME cho_util_python)

# target_include_directories( cho_util_python PUBLIC
# $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
# set_target_properties(cho_util_python PROPERTIES LINKER_LANGUAGE CXX)
