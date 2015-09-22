#ifndef ct_helpers_hpp
#define ct_helpers_hpp

namespace thls
{

template<
      bool C
    , typename T1
    , typename T2
    >
struct if_c
{
    typedef T1 type;
};

template<
      typename T1
    , typename T2
    >
struct if_c<false,T1,T2>
{
    typedef T2 type;
};


template<
    bool C1,        typename T1,
    bool C2,        typename T2,
    bool C3=false,  typename T3=void,
    bool C4=false,  typename T4=void,
    bool C5=false,  typename T5=void
>
struct select_c
{
    struct error_no_condition_true
    {
      HLS_STATIC_ASSERT(0, "No condition was true.");  
    };
    
    typedef typename if_c<
        C1, T1,
        typename if_c<
            C2, T2,
            typename if_c<
                C3, T3,
                typename if_c<
                    C4, T4,
                    typename if_c<
                        C5, T5,
                        error_no_condition_true::type 
                    >::type
                >::type
            >::type
        >::type
    >::type type;
};

}; // thls

#endif

