#include "thls/types/intval.hpp"

int64_t add_ival_uint8(int64_t _a, int64_t _b);

#include <cstdio>

int main()
{
    for(unsigned x=0;x<256;x++){
        for(unsigned y=0;y<256;y++){
            unsigned r=x+y;
            int64_t g=add_ival_uint8(x,y);
            
            if(add_ival_uint8(x,y)!=r){
                fprintf(stderr, "%u+%u=%u, got=%u\n", x,y,r,g);
                return 1;
            }
            
            r=x*y;
            int64_t g=mul_ival_uint8(x,y);
            
            if(mul_ival_uint8(x,y)!=r){
                fprintf(stderr, "%u*%u=%u, got=%u\n", x,y,r,g);
                return 1;
            }
        }
    }
    
    fprintf(stderr, "Success\n");
    
    return 0;
}
