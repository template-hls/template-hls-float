#ifndef policy_ieee_on_flopoco_hpp
#define policy_ieee_on_flopoco_hpp

#include "thls/tops/fp_ieee.hpp"
#include "thls/tops/fp_flopoco.hpp"
#include "thls/tops/fp_convert.hpp"

namespace thls
{

template<int wE,int wF>
struct fp_ieee_on_flopoco_wrapper
{
private:
    typedef fp_flopoco<wE,wF> internal_t;

    static internal_t in(const raw_t &x)
    {
        internal_t res;
        convert(res,x);
        return res;
    }
    
    static raw_t out(const internal_t &x)
    {
        raw_t res;
        convert(res,x);
        return res;
    }
public:
    typedef fp_ieee<wE,wF> raw_t;
    
    raw_t raw;
    
    fp_ieee_on_flopoco_wrapper(const raw_t &x)
        : raw(x)
    {}
    
    fp_ieee_on_flopoco_wrapper operator*(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(mul<wE,wF>(in(raw), in(b.raw))); }
    
    fp_ieee_on_flopoco_wrapper operator+(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(add_single<wE,wF>(in(raw), in(b.raw)); }
    
    
    fp_ieee_on_flopoco_wrapper operator<(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(less_than(in(raw), in(b.raw))); }
    
    fp_ieee_on_flopoco_wrapper operator<=(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(less_than_equal(in(raw), in(b.raw))); }
    
    fp_ieee_on_flopoco_wrapper operator==(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(equal(in(raw), in(b.raw))); }
    
    fp_ieee_on_flopoco_wrapper operator>=(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(greater_than_equal(in(raw), in(b.raw))); }
    
    fp_ieee_on_flopoco_wrapper operator>(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(greater_than(in(raw), in(b.raw))); }
    
    fp_ieee_on_flopoco_wrapper operator!=(const fp_ieee_on_flopoco_wrapper &b) const
    { return out(not_equal(in(raw), in(b.raw))); }
};

template<int wE,int wF>
struct policy_ieee_on_flopoco
{
    typedef fp_ieee<wE,wF> value_t;
    
    typedef fw_uint<wE+wF+1> external_t;
    
    static value_t from_external(external_t x)
    { return value_t(x); }
    
    static external_t to_external(value_t x)
    { return x.bits; }
};

}; // thls

#endif
