
configure_file(fp_add_native_nodsp_5_10.cpp fp_add_native_nodsp_5_10.cpp COPYONLY)
configure_file(fp_add_native_5_10.cpp fp_add_native_5_10.cpp COPYONLY)
configure_file(fp_mul_native_5_10.cpp fp_mul_native_5_10.cpp COPYONLY)
configure_file(fp_div_native_5_10.cpp fp_div_native_5_10.cpp COPYONLY)

configure_file(fp_add_native_8_23.cpp fp_add_native_8_23.cpp COPYONLY)
configure_file(fp_add_native_nodsp_8_23.cpp fp_add_native_nodsp_8_23.cpp COPYONLY)
configure_file(fp_add_native_fulldsp_8_23.cpp fp_add_native_fulldsp_8_23.cpp COPYONLY)
configure_file(fp_mul_native_8_23.cpp fp_mul_native_8_23.cpp COPYONLY)
configure_file(fp_mul_native_nodsp_8_23.cpp fp_mul_native_nodsp_8_23.cpp COPYONLY)
configure_file(fp_mul_native_meddsp_8_23.cpp fp_mul_native_meddsp_8_23.cpp COPYONLY)
configure_file(fp_mul_native_fulldsp_8_23.cpp fp_mul_native_fulldsp_8_23.cpp COPYONLY)
configure_file(fp_mul_native_maxdsp_8_23.cpp fp_mul_native_maxdsp_8_23.cpp COPYONLY)
configure_file(fp_div_native_8_23.cpp fp_div_native_8_23.cpp COPYONLY)

configure_file(fp_add_native_nodsp_11_52.cpp fp_add_native_nodsp_11_52.cpp COPYONLY)
configure_file(fp_add_native_11_52.cpp fp_add_native_11_52.cpp COPYONLY)
configure_file(fp_mul_native_11_52.cpp fp_mul_native_11_52.cpp COPYONLY)
configure_file(fp_div_native_11_52.cpp fp_div_native_11_52.cpp COPYONLY)

add_custom_target(all_generated_primitive_cpp)



function(fp_primitive_add_instance E F)
set(EEE ${E})
set(FFF ${F})

configure_file( fp_mul_flopoco_EEE_FFF.cpp fp_mul_flopoco_${EEE}_${FFF}.cpp )

configure_file( fp_add_flopoco_single_EEE_FFF.cpp fp_add_flopoco_single_${EEE}_${FFF}.cpp )
configure_file( fp_add_flopoco_single_v2_EEE_FFF.cpp fp_add_flopoco_single_v2_${EEE}_${FFF}.cpp )
configure_file( fp_add_flopoco_single_v3_EEE_FFF.cpp fp_add_flopoco_single_v3_${EEE}_${FFF}.cpp )
configure_file( fp_add_flopoco_single_v4_EEE_FFF.cpp fp_add_flopoco_single_v4_${EEE}_${FFF}.cpp )

configure_file( fp_add_flopoco_dual_EEE_FFF.cpp fp_add_flopoco_dual_${EEE}_${FFF}.cpp )

configure_file( fp_div_flopoco_EEE_FFF.cpp fp_div_flopoco_${EEE}_${FFF}.cpp )
configure_file( fp_div_flopoco_v2_EEE_FFF.cpp fp_div_flopoco_v2_${EEE}_${FFF}.cpp )
configure_file( fp_div_flopoco_v3_EEE_FFF.cpp fp_div_flopoco_v3_${EEE}_${FFF}.cpp )


configure_file( fp_mul_ieee_on_flopoco_EEE_FFF.cpp fp_mul_ieee_on_flopoco_${EEE}_${FFF}.cpp )
configure_file( fp_div_ieee_on_flopoco_EEE_FFF.cpp fp_div_ieee_on_flopoco_${EEE}_${FFF}.cpp )
configure_file( fp_add_ieee_on_flopoco_EEE_FFF.cpp fp_add_ieee_on_flopoco_${EEE}_${FFF}.cpp )

endfunction(fp_primitive_add_instance)

foreach(E RANGE 4 13)
    foreach(F RANGE 4 112)
        fp_primitive_add_instance(${E} ${F})
    endforeach(F)
endforeach(E)


function(fp_primitive_ratio_add_instance E1 F1 E2 F2)
set(EEE1 ${E1})
set(FFF1 ${F1})
set(EEE2 ${E2})
set(FFF2 ${F2})

configure_file( fp_add_flopoco_single_ratio_EEE1_FFF1_EEE2_FFF2.cpp fp_add_flopoco_single_ratio_${EEE1}_${FFF1}_${EEE2}_${FFF2}.cpp )
configure_file( fp_mul_flopoco_ratio_EEE1_FFF1_EEE2_FFF2.cpp fp_mul_flopoco_ratio_${EEE1}_${FFF1}_${EEE2}_${FFF2}.cpp )
endfunction(fp_primitive_ratio_add_instance)

foreach(F1 RANGE 3 44 2)
fp_primitive_ratio_add_instance(8 23 8 ${F1})
endforeach(F1)

foreach(F1 RANGE 4 104 4)
fp_primitive_ratio_add_instance(11 52 11 ${F1})
endforeach(F1)


add_executable(fp_primitive_build_main build_main.cpp
    fp_mul_native_8_23.cpp
    fp_add_native_8_23.cpp
    fp_div_native_8_23.cpp

    fp_mul_native_11_52.cpp
    fp_add_native_11_52.cpp
    fp_div_native_11_52.cpp

    fp_add_softfloat_8_23.cpp
    fp_mul_softfloat_8_23.cpp
    fp_div_softfloat_8_23.cpp

    fp_mul_flopoco_8_23.cpp
    fp_add_flopoco_single_8_23.cpp
    fp_add_flopoco_dual_8_23.cpp
        fp_div_flopoco_8_23.cpp

        fp_add_ieee_on_flopoco_8_23.cpp
        fp_mul_ieee_on_flopoco_8_23.cpp
        fp_div_ieee_on_flopoco_8_23.cpp

        )
        
target_link_libraries(fp_primitive_build_main stdc++)

# The vivado top-levels have pragmas on them
target_compile_options(fp_primitive_build_main PUBLIC "-Wno-unknown-pragmas" )
