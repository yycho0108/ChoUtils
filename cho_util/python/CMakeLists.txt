# hmmm ...
add_subdirectory(pybind11)

pybind11_add_module(cho_util_python src/module.cpp)
