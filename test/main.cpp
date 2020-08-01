#include "cho_util/core/version.hpp"
#include "cho_util/core/math.hpp"
#include "cho_util/core/random.hpp"
#include "cho_util/util/timer.hpp"

#include <iostream>

int main()
{
    std::cout << CHO_VERSION_MAJOR << std::endl;
    std::cout << CHO_VERSION_MINOR << std::endl;
    std::cout << CHO_VERSION_PATCH << std::endl;

    cho::util::MTimer timer{ true };
    cho::core::RNG rng;
    const std::vector<float> times = cho::core::Linspace(0, 100, 100);
    for (const auto& x : times) {
        std::cout << x << std::endl;
    }
    rng.SetSeed(1023123);
    std::cout << rng.Randu() << std::endl;
    std::cout << rng.Randn() << std::endl;
    std::cout << timer.StopAndGetElapsedTime() << std::endl;
}
