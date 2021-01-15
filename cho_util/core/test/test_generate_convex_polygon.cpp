#include "cho_util/core/random_convex_polygon.hpp"

#include <cstdlib>
#include <iostream>

#include <fmt/printf.h>
#include <boost/program_options.hpp>

#include "cho_util/core/random.hpp"

namespace po = boost::program_options;

struct AppSettings {
  int num_points{256};
  int seed{0};
};

void Usage(const po::options_description& desc) {
  // clang-format off
  fmt::print(R"(Usage:
  test_generate_convex_polygon [options]
Description:
  Test convex polygon generation.
Options:
{}
  )",
             desc);
  // clang-format on
}

bool ParseArguments(int argc, char* argv[], AppSettings* const settings) {
  po::options_description desc("");

  // clang-format off
  desc.add_options()
      ("help,h", "help")
      ("num_points,n", po::value<int>(&settings->num_points)->default_value(256), "Number of points in convex hull.")
      ("seed,s", po::value<int>(&settings->seed)->default_value(0), "Random RNG seed.")
      ;
  // clang-format on
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    Usage(desc);
    return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
  AppSettings settings;
  if (!ParseArguments(argc, argv, &settings)) {
    return 1;
  }
  auto& rng = cho::core::RNG::GetInstance();
  if (settings.seed >= 0) {
    rng.SetSeed(settings.seed);
  }
  std::vector<Eigen::Vector2f> points;
  cho::core::GenerateConvexPolygon(settings.num_points, &points, rng);

  for (const auto& p : points) {
    std::cout << p.transpose() << std::endl;
  }
  return 0;
}
