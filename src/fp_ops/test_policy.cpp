#include "thls/tops/policy_test.hpp"

#include <random>
#include <iostream>

#define CHECK(x) if(!(x)){ fprintf(stderr, "Fail : %s, %d - %s\n", __FILE__, __LINE__, #x); exit(1); }

#define CHECK_EQUAL_FW(x,y) if((x!=y).to_bool()){ std::cerr<<"Fail : "<<__FILE__<<" , "<<__LINE__<<"\n  x="<<x<<"\n  y="<<y<<"\n"; exit(1); }

using namespace thls;

int main()
{
    try{
        test_policy<policy_concept>::test_arithmetic();
        
        fprintf(stderr, "Done\n");
    }catch(std::exception &e){
        std::cerr<<"Exception : "<<e.what()<<"\n";
        exit(1);
    }

    return 0;
}
