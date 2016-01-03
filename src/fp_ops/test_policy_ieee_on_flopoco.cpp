#include <random>
#include <iostream>


#include "thls/tops/policy_ieee_on_flopoco.hpp"


#define CHECK(x) if(!(x)){ fprintf(stderr, "Fail : %s, %d - %s\n", __FILE__, __LINE__, #x); exit(1); }

#define CHECK_EQUAL_FW(x,y) if((x!=y).to_bool()){ std::cerr<<"Fail : "<<__FILE__<<" , "<<__LINE__<<"\n  x="<<x<<"\n  y="<<y<<"\n"; exit(1); }

using namespace thls;

extern "C" void sig_handler(int s)
{
    fprintf(stderr, "Signal: %d", s);
    exit(1);
}

int main()
{
    signal(SIGFPE, sig_handler);
    signal(SIGSEGV	, sig_handler);
    signal(SIGABRT	, sig_handler);

    try{
        fprintf(stderr, "Single...\n");
        test_policy<policy_ieee_on_flopoco<8,23> >::test_arithmetic();
        fprintf(stderr, "Double...\n");
        test_policy<policy_ieee_on_flopoco<11,52> >::test_arithmetic();
        fprintf(stderr, "Half..\n");
        test_policy<policy_ieee_on_flopoco<5,10> >::test_arithmetic();
        
        fprintf(stderr, "Done\n");
    }catch(std::exception &e){
        std::cerr<<"Exception : "<<e.what()<<"\n";
        exit(1);
    }

    return 0;
}
