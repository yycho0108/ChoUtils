#pragma once

#include "cho_util/proto/render.grpc.pb.h"
#include "cho_util/proto/render.pb.h"
#include "cho_util/type/convert.hpp"
#include "cho_util/vis/render_data.hpp"

namespace cho {
namespace type {

template <>
CHO_DECLARE_CONVERT(cho::proto::vis::render::RenderType, cho::vis::RenderType);

template <>
CHO_DECLARE_CONVERT(cho::vis::RenderType, cho::proto::vis::render::RenderType);

template <>
CHO_DECLARE_CONVERT(cho::proto::vis::render::RepresentationType,
                    cho::vis::RenderData::Representation);
template <>
CHO_DECLARE_CONVERT(cho::vis::RenderData::Representation,
                    cho::proto::vis::render::RepresentationType);

template <>
CHO_DECLARE_CONVERT(cho::vis::RenderData,
                    cho::proto::vis::render::RenderRequest);

template <>
CHO_DECLARE_CONVERT(cho::proto::vis::render::RenderRequest,
                    cho::vis::RenderData);

template <typename Derived>
void Convert(const proto::vis::geometry::Vector3f& source,
             Eigen::DenseBase<Derived>* const target) {
  (*target)(0) = source.x();
  (*target)(1) = source.y();
  (*target)(2) = source.z();
}
template <typename Derived>
void Convert(const Eigen::DenseBase<Derived>& source,
             proto::vis::geometry::Vector3f* const target) {
  target->set_x(source(0));
  target->set_y(source(1));
  target->set_z(source(2));
}

template <typename Scalar>
struct Converter<proto::vis::geometry::Cuboid, core::Cuboid<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::Cuboid& source,
                        core::Cuboid<Scalar, 3>* const target) {
    auto& dst = target->GetData();
    switch (source.bounds_case()) {
      case proto::vis::geometry::Cuboid::kMinMax: {
        auto blk_min = dst.col(0);
        auto blk_max = dst.col(1);
        Convert(source.min_max().min(), &blk_min);
        Convert(source.min_max().max(), &blk_max);
        break;
      }
      case proto::vis::geometry::Cuboid::kCenterRadius: {
        Eigen::Vector3f center, radius;
        Convert(source.center_radius().center(), &center);
        Convert(source.center_radius().radius(), &radius);
        dst.col(0) = center - Scalar{0.5} * radius;
        dst.col(1) = center + Scalar{0.5} * radius;
        break;
      }
      default: {
        // throw std::out_of_range(
        //    fmt::format("Unsupported bounds : {}", source.bounds_case()));
        break;
      }
    }
  }
};

template <typename Scalar>
struct Converter<core::Cuboid<Scalar, 3>, proto::vis::geometry::Cuboid,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::Cuboid<Scalar, 3>& source,
                        proto::vis::geometry::Cuboid* const target) {
    const auto& src = source.GetData();
    Convert(src.col(0), target->mutable_min_max()->mutable_min());
    Convert(src.col(1), target->mutable_min_max()->mutable_max());
  }
};

template <typename Scalar>
struct Converter<proto::vis::geometry::PointCloud, core::PointCloud<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::PointCloud& source,
                        core::PointCloud<Scalar, 3>* const target) {
    const int n = source.points_size();
    target->GetData().resize(n, 3);

    for (int i = 0; i < n; ++i) {
      const auto& src = source.points(i);
      auto dst = target->GetData().col(i);
      dst.x() = src.x();
      dst.y() = src.y();
      dst.z() = src.z();
    }
  }
};

template <typename Scalar>
struct Converter<core::PointCloud<Scalar, 3>, proto::vis::geometry::PointCloud,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::PointCloud<Scalar, 3>& source,
                        proto::vis::geometry::PointCloud* const target) {
    const auto& src = source.GetData();
    target->mutable_points()->Reserve(src.cols());
    for (int i = 0; i < src.cols(); ++i) {
      auto point = target->mutable_points()->Add();
      Convert(src.col(i), point);
    }
  }
};

template <typename Scalar>
struct Converter<proto::vis::geometry::Sphere, core::Sphere<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::Sphere& source,
                        core::Sphere<Scalar, 3>* const target) {
    auto blk_ctr = target->GetCenter();
    Convert(source.center(), &blk_ctr);
    target->SetRadius(source.radius());
  }
};

template <typename Scalar>
struct Converter<core::Sphere<Scalar, 3>, proto::vis::geometry::Sphere,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::Sphere<Scalar, 3>& source,
                        proto::vis::geometry::Sphere* const target) {
    Convert(source.GetCenter(), target->mutable_center());
    target->set_radius(source.GetRadius());
  }
};

template <typename Scalar>
struct Converter<proto::vis::geometry::Vector3f, core::Point<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::Vector3f& source,
                        core::Point<Scalar, 3>* const target) {
    auto& dst = target->GetData();
    Convert(source, dst);
  }
};

template <typename Scalar>
struct Converter<core::Point<Scalar, 3>, proto::vis::geometry::Vector3f,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::Point<Scalar, 3>& source,
                        proto::vis::geometry::Vector3f* const target) {
    const auto& src = source.GetData();
    Convert(src, target);
  }
};

template <typename Scalar>
struct Converter<proto::vis::geometry::Line, core::Line<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::Line& source,
                        core::Line<Scalar, 3>* const target) {
    auto& dst = target->GetData();
    Convert(source.start(), dst.col(0));
    Convert(source.end(), dst.col(1));
  }
};

template <typename Scalar>
struct Converter<core::Line<Scalar, 3>, proto::vis::geometry::Line,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::Line<Scalar, 3>& source,
                        proto::vis::geometry::Line* const target) {
    auto& src = source.GetData();
    Convert(src.col(0), target->mutable_start());
    Convert(src.col(1), target->mutable_end());
  }
};

template <typename Scalar>
struct Converter<proto::vis::geometry::Lines, core::Lines<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::Lines& source,
                        core::Lines<Scalar, 3>* const target) {
    auto& dst = target->GetData();
    dst.resize(source.lines_size() * 3, 2);
    for (int i = 0; i < source.lines_size(); ++i) {
      const auto& line = source.lines(i);

      auto block = dst.template block<3, 2>(i, 0);
      auto blk0 = block.col(0);
      auto blk1 = block.col(1);
      Convert(line.start(), &blk0);
      Convert(line.end(), &blk1);
    }
  }
};

template <typename Scalar>
struct Converter<core::Lines<Scalar, 3>, proto::vis::geometry::Lines,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::Lines<Scalar, 3>& source,
                        proto::vis::geometry::Lines* const target) {
    auto& src = source.GetData();
    const int n = src.rows() / 3;
    target->mutable_lines()->Reserve(n);
    for (int i = 0; i < n; ++i) {
      const auto s = src.template block<3, 1>(i * 3, 0);
      const auto e = src.template block<3, 1>(i * 3, 1);
      auto line = target->mutable_lines(i);
      Convert(Eigen::Vector3f{s}, line->mutable_start());
      Convert(Eigen::Vector3f{e}, line->mutable_end());
    }
  }
};

template <typename Scalar>
struct Converter<proto::vis::geometry::Plane, core::Plane<Scalar, 3>,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const proto::vis::geometry::Plane& source,
                        core::Plane<Scalar, 3>* const target) {
    auto blk_ctr = target->GetCenter();
    auto blk_nrm = target->GetNormal();
    Convert(source.center(), &blk_ctr);
    Convert(source.normal(), &blk_nrm);
  }
};

template <typename Scalar>
struct Converter<core::Plane<Scalar, 3>, proto::vis::geometry::Plane,
                 std::enable_if_t<std::is_arithmetic<Scalar>::value>> {
  static void ConvertTo(const core::Plane<Scalar, 3>& source,
                        proto::vis::geometry::Plane* const target) {
    Convert(source.GetCenter(), target->mutable_center());
    Convert(source.GetNormal(), target->mutable_normal());
  }
};

}  // namespace type
}  // namespace cho
