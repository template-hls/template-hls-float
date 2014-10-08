#ifndef rom_hpp
#define rom_hpp

template<class ...Tv>
struct rom_output_type;

template<class Tv0>
struct rom_output_type<Tv0>
{ typedef Tv0 type; };

template<class Tv0, class ...Tvr>
struct rom_output_type<Tv0,Tvr>
{
	typedef rom_output_type<Tvr>::type base_type;

	typedef union_type<Tv0,base_type>::type type;
};

#error "How should this work?"

template<class ...Tv>
typename rom_output_type<Tv>::type rom(
	unsigned index
){
	typedef rom_output_type<Tv>::type res_t;

	std::array<res_t,sizeof...(Tv)> values(
		...
	);
}

#endif
