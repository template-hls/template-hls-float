#include "thls/types/intval.hpp"

int64_t add_ival_uint8(int64_t _a, int64_t _b);
int64_t mul_ival_uint8(int64_t _a, int64_t _b);

int64_t sr_0_ival_uint8(int64_t _a);    
int64_t sr_1_ival_uint8(int64_t _a);    
int64_t sr_7_ival_uint8(int64_t _a);    
int64_t sr_8_ival_uint8(int64_t _a);    


#include <cstdio>

int main()
{
    for(unsigned x=0;x<256;x++){
        for(unsigned y=0;y<256;y++){
            unsigned r=x+y;
            int64_t g=add_ival_uint8(x,y);
            
            if(g!=r){
                fprintf(stderr, "%u+%u=%u, got=%u\n", x,y,r,g);
                return 1;
            }
            
            r=x*y;
            g=mul_ival_uint8(x,y);
            
            if(g!=r){
                fprintf(stderr, "%u*%u=%u, got=%u\n", x,y,r,g);
                return 1;
            }
            
            r=x>>0;
            g=sr_0_ival_uint8(x);
            
            if(g!=r){
                fprintf(stderr, "%u>>0=%u, got=%u\n", x,r,g);
                return 1;
            }
            
            r=x>>1;
            g=sr_1_ival_uint8(x);
            
            if(g!=r){
                fprintf(stderr, "%u>>1=%u, got=%u\n", x,r,g);
                return 1;
            }
            
            r=x>>7;
            g=sr_7_ival_uint8(x);
            
            if(g!=r){
                fprintf(stderr, "%u>>7=%u, got=%u\n", x,r,g);
                return 1;
            }
            
            r=x>>8;
            g=sr_8_ival_uint8(x);
            
            if(g!=r){
                fprintf(stderr, "%u>>8=%u, got=%u\n", x,r,g);
                return 1;
            }
        }
    }
    
    fprintf(stderr, "Success\n");
    
    return 0;
}
