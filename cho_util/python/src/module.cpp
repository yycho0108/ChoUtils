
#include <fstream>
#include <iostream>

#include <pybind11/pybind11.h>

#include "cho_util/core/geometry/point_cloud.hpp"
#include "cho_util/type/convert.hpp"
#include "cho_util/vis/convert_proto.hpp"
#include "cho_util/vis/subprocess_viewer.hpp"

namespace py = pybind11;

static cho::core::PointCloud<float, 3> ReadCloud(const std::string& filename) {
  std::ifstream fin{filename, std::ios_base::binary | std::ios_base::in};
  cho::proto::core::geometry::PointCloud cloud_proto;
  cloud_proto.ParseFromIstream(&fin);
  const auto& cloud_raw =
      cho::type::Convert<cho::core::PointCloud<float, 3>>(cloud_proto);
  return cloud_raw;
}

PYBIND11_MODULE(cho_util_python, m) {
  m.doc() = "cho_util python plugin";

  // Add Cloud3f class.
  py::class_<cho::core::PointCloud<float, 3>>(m, "Cloud3f",
                                              py::buffer_protocol())
      .def(py::init<>())
      .def_buffer([](cho::core::PointCloud<float, 3>& m) -> py::buffer_info {
        return py::buffer_info(
            m.GetPtr(), sizeof(float), py::format_descriptor<float>::format(),
            2, {m.GetData().rows(), m.GetData().cols()},
            {sizeof(float), sizeof(float) * m.GetData().rows()});
      });

  // Add protobuf reader ...
  m.def("read_cloud_pb", &ReadCloud, "Read point cloud from protobuf filename");
}
