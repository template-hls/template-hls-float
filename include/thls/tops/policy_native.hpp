#ifndef policy_native_hpp
#define policy_native_hpp

struct policy_native_single
{
    typedef float value_t;
    
    typedef float external_t;
    
    static value_t from_external(external_t x)
    { return x; }
    
    static external_t to_external(value_t x)
    { return x; }
};

struct policy_native_double
{
    typedef double value_t;
    
    typedef double external_t;
    
    static value_t from_external(external_t x)
    { return x; }
    
    static external_t to_external(value_t x)
    { return x; }
};

#endif
