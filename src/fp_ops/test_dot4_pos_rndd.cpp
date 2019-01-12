#include "thls/tops/fp_flopoco_dot4_pos_rndd.hpp"

#include "thls/tops/make_input.hpp"

#include <random>
#include <cmath>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;


int main()
{

	//exhaust_dot4_pos_rndd<3,1>();

	test_dot4_pos_rndd<1,3>(1000000);
	test_dot4_pos_rndd<2,3>(1000000);
	test_dot4_pos_rndd<3,3>(1000000);
	test_dot4_pos_rndd<4,3>(1000000);
	test_dot4_pos_rndd<3,4>(1000000);
	test_dot4_pos_rndd<4,4>(10000000);
	test_dot4_pos_rndd<5,4>(10000000);
	test_dot4_pos_rndd<6,4>(10000000);
	test_dot4_pos_rndd<4,5>(10000000);
	test_dot4_pos_rndd<5,5>(10000000);
	test_dot4_pos_rndd<6,5>(100000000);

	test_dot4_pos_rndd<4,16>(100000000);
	test_dot4_pos_rndd<5,16>(100000000);
	test_dot4_pos_rndd<6,16>(100000000);
	
	

    return 0;
}

