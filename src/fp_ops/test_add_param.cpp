#include "thls/tops/fp_flopoco_add_single_v1.hpp"

#include "thls/tops/make_input.hpp"

#include <random>
#include <cmath>
#include <fstream>

std::mt19937 rng;
std::uniform_real_distribution<float> urng;
std::normal_distribution<float> grng;
std::exponential_distribution<float> erng;

using namespace thls;

template<class TType,class TImpl>
bool test_add(const TImpl &impl, const TType &fa, const TType &fb, std::ostream &dst)
{
	TType fgot=impl(fa,fb, 0); // Version without debug output
	TType fref;
	ref_add(fref, fa, fb);

	auto raw=[](std::string x)
	{
		if(x.substr(0,2)=="0b"){
			x=x.substr(2);
		}
		
		while(x.find('_')!=std::string::npos){
			x.erase(x.find('_'), 1);
		}
		
		return x;
	};

	if(!fref.equals(fgot).to_bool()){
		dst<<raw(fa.bits.to_string())<<" "<<raw(fb.bits.to_string())<<" \n";
		dst<<"1 "<<raw(fref.bits.to_string())<<" \n";
		return true;
	}else{
		return false;
	}
}

template<class TType ,class TImpl>
std::pair<int,int> test_impl(TImpl &impl, int n, std::ostream &dst)
{
	std::vector<TType> args;
	
	make_input(args, n);
	
	int fails=0, total=0;

	for(unsigned i=0; i<args.size(); i++){
		std::cerr<<"i = "<<i<<"\n";
		for(unsigned j=0; j<args.size(); j++){
			if(test_add<TType >(impl, args[i], args[j], dst)){
				fails++;
			}
			total++;
		}
	}
	
	return std::make_pair(fails,total);
}


int main(int argc, char *argv[])
{
	static const int E= __E__;
	static const int F= __F__;
	
	std::ostream *dst=&std::cerr;
	
	std::ofstream dstFile;
    if(argc>1){
        if(strcmp(argv[1],"-")){
            dstFile.open(argv[1]);
            if(!dstFile.is_open()){
                fprintf(stderr, "Couldn't open dest '%s'\n", argv[1]);
                exit(1);
            }
            dst=&dstFile;
        }
    }
    
    int n=200;
    if(argc>2){
        n=atoi(argv[2]);
    }
	

	if( (1<<E) > F){ 
		auto r=test_impl<fp_flopoco<E,F>>(add<E,F,E,F,E,F>, n, *dst);
		fprintf(stdout, "%d, %d, %s, %d, %d, %d", E, F, r.first==0?"pass":"fail", n, r.first, r.second);
	}else{
		fprintf(stdout, "%d, %d, invalid, 0, 0, 0", E, F);
	}


    return 0;
}

