#include "thls/tops/fp_flopoco_dot2_pos_rndd.hpp"

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
	/*
	exhaust_dot2_pos_rndd<3,3>();
	exhaust_dot2_pos_rndd<3,4>();
	exhaust_dot2_pos_rndd<4,3>();
	*/
	test_dot2_pos_rndd<8,7>(100000);
	test_dot2_pos_rndd<8,22>(100000);
	test_dot2_pos_rndd<8,23>(100000);
	test_dot2_pos_rndd<8,24>(100000);
	test_dot2_pos_rndd<5,10>(100000);	
	test_dot2_pos_rndd<11,51>(100000);	
	test_dot2_pos_rndd<11,52>(100000);	
	test_dot2_pos_rndd<11,53>(100000);	
	test_dot2_pos_rndd<11,54>(100000);	
	test_dot2_pos_rndd<11,60>(100000);	

    return 0;
}

