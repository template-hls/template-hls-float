#ifndef policy_flopoco_hpp
#define policy_flopoco_hpp


template<int wE,int wF>
struct fp_flopoco_wrapper
{
    typedef fp_flopoco<wE,wF> raw_t;
    
    raw_t raw;
    
    fp_flopoco_wrapper(const raw_t &x)
        : raw(x)
    {}
    
    fp_flopoco_wrapper operator*(const fp_flopoco_wrapper &b) const
    { return mul<wE,wF>(raw, b.raw); }
    
    fp_flopoco_wrapper operator+(const fp_flopoco_wrapper &b) const
    { return add_single<wE,wF>(raw, b.raw); }
    
    
    fp_flopoco_wrapper operator<(const fp_flopoco_wrapper &b) const
    { return less_than(raw, b.raw); }
    
    fp_flopoco_wrapper operator<=(const fp_flopoco_wrapper &b) const
    { return less_than_equal(raw, b.raw); }
    
    fp_flopoco_wrapper operator==(const fp_flopoco_wrapper &b) const
    { return equal(raw, b.raw); }
    
    fp_flopoco_wrapper operator>=(const fp_flopoco_wrapper &b) const
    { return greater_than_equal(raw, b.raw); }
    
    fp_flopoco_wrapper operator>(const fp_flopoco_wrapper &b) const
    { return greater_than(raw, b.raw); }
    
    fp_flopoco_wrapper operator!=(const fp_flopoco_wrapper &b) const
    { return not_equal(raw, b.raw); }
};

template<int wE,int wF>
struct policy_flopoco
{
    typedef fp_flopoco<wE,wF> value_t;
    
    typedef fw_uint<wE+wF+3> external_t;
    
    static value_t from_external(external_t x)
    { return value_t(x); }
    
    static external_t to_external(value_t x)
    { return x.bits; }
};

#endif
