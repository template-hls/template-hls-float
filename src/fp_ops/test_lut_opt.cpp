#include "thls/tops/lut_opt.hpp"

#include <random>

#include <cmath>

#include <stdio.h>
#include <mpfr.h>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

int main() {
    static const int N = 16;
    static const int T = 6;

    typedef lut_opt_spec<N, T, 1> lut_opt;

    fw_uint<N> s(0b101);
    for (int i = 0; i < 10; i++) {
        std::cout<<s<<"\n";
        s = lut_opt::step(s);

    }

    return 0;
}

