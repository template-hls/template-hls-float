#error "This is in progress, and doesn't currently work"
#include "thls/tops/fp_flopoco_dot2_pos_rndn.hpp"

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

	test_dot2_pos_rndn<3,3>();
	/*test_dot2_pos_rndn<3,4>();
	test_dot2_pos_rndn<4,3>();
	test_dot2_pos_rndn<4,4>();
	test_dot2_pos_rndn<4,5>();	*/

    return 0;
}

