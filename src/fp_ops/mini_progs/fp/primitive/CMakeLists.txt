

configure_file(fp_add_native_8_23.cpp fp_add_native_8_23.cpp COPYONLY)
configure_file(fp_mul_native_8_23.cpp fp_mul_native_8_23.cpp COPYONLY)
configure_file(fp_div_native_8_23.cpp fp_div_native_8_23.cpp COPYONLY)

configure_file(fp_add_native_10_52.cpp fp_add_native_10_52.cpp COPYONLY)
configure_file(fp_mul_native_10_52.cpp fp_mul_native_10_52.cpp COPYONLY)
configure_file(fp_div_native_10_52.cpp fp_div_native_10_52.cpp COPYONLY)

add_custom_target(all_generated_primitive_cpp)

function(fp_primitive_add_instance EEE FFF )

add_custom_command(
    OUTPUT fp_mul_flopoco_${EEE}_${FFF}.cpp
    COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_mul_flopoco_EEE_FFF.cpp > fp_mul_flopoco_${EEE}_${FFF}.cpp
    DEPENDS fp_mul_flopoco_EEE_FFF.cpp
)
add_custom_target(fp_mul_flopoco_${EEE}_${FFF}_src
    DEPENDS fp_mul_flopoco_${EEE}_${FFF}.cpp
)
add_dependencies(all_generated_primitive_cpp fp_mul_flopoco_${EEE}_${FFF}_src)

add_custom_command(
    OUTPUT fp_add_flopoco_single_${EEE}_${FFF}.cpp
    COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_add_flopoco_single_EEE_FFF.cpp > fp_add_flopoco_single_${EEE}_${FFF}.cpp
    DEPENDS fp_add_flopoco_single_EEE_FFF.cpp
)
add_custom_target(fp_add_flopoco_single_${EEE}_${FFF}_src
    DEPENDS fp_add_flopoco_single_${EEE}_${FFF}.cpp
)
add_dependencies(all_generated_primitive_cpp fp_add_flopoco_single_${EEE}_${FFF}_src)

add_custom_command(
    OUTPUT fp_add_flopoco_dual_${EEE}_${FFF}.cpp
    COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_add_flopoco_dual_EEE_FFF.cpp > fp_add_flopoco_dual_${EEE}_${FFF}.cpp
    DEPENDS fp_add_flopoco_dual_EEE_FFF.cpp
)
add_custom_target(fp_add_flopoco_dual_${EEE}_${FFF}_src
    DEPENDS fp_add_flopoco_dual_${EEE}_${FFF}.cpp
)
add_dependencies(all_generated_primitive_cpp fp_add_flopoco_dual_${EEE}_${FFF}_src)

add_custom_command(
        OUTPUT fp_div_flopoco_${EEE}_${FFF}.cpp
        COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_div_flopoco_EEE_FFF.cpp > fp_div_flopoco_${EEE}_${FFF}.cpp
        DEPENDS fp_div_flopoco_EEE_FFF.cpp
)
add_custom_target(fp_div_flopoco_${EEE}_${FFF}_src
    DEPENDS fp_div_flopoco_${EEE}_${FFF}.cpp
)
add_dependencies(all_generated_primitive_cpp fp_div_flopoco_${EEE}_${FFF}_src)


add_custom_command(
        OUTPUT fp_div_flopoco_v2_${EEE}_${FFF}.cpp
        COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_div_flopoco_v2_EEE_FFF.cpp > fp_div_flopoco_v2_${EEE}_${FFF}.cpp
        DEPENDS fp_div_flopoco_v2_EEE_FFF.cpp
)
add_custom_target(fp_div_flopoco_v2_${EEE}_${FFF}_src
    DEPENDS fp_div_flopoco_v2_${EEE}_${FFF}.cpp
)
add_dependencies(all_generated_primitive_cpp fp_div_flopoco_v2_${EEE}_${FFF}_src)

#add_custom_command(
#        OUTPUT fp_div_flopoco_v3_${EEE}_${FFF}.cpp
#        COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_div_flopoco_v3_EEE_FFF.cpp > fp_div_flopoco_v3_${EEE}_${FFF}.cpp
#        DEPENDS fp_div_flopoco_v3_EEE_FFF.cpp
#)
#add_custom_target(fp_div_flopoco_v3_${EEE}_${FFF}_src
#    DEPENDS fp_div_flopoco_v3_${EEE}_${FFF}.cpp
#)
#add_dependencies(all_generated_primitive_cpp fp_div_flopoco_v3_${EEE}_${FFF}_src)


add_custom_command(
        OUTPUT fp_add_ieee_on_flopoco_${EEE}_${FFF}.cpp
        COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_add_ieee_on_flopoco_EEE_FFF.cpp > fp_add_ieee_on_flopoco_${EEE}_${FFF}.cpp
        DEPENDS fp_add_ieee_on_flopoco_EEE_FFF.cpp
)

add_custom_command(
        OUTPUT fp_mul_ieee_on_flopoco_${EEE}_${FFF}.cpp
        COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_mul_ieee_on_flopoco_EEE_FFF.cpp > fp_mul_ieee_on_flopoco_${EEE}_${FFF}.cpp
        DEPENDS fp_mul_ieee_on_flopoco_EEE_FFF.cpp
)

add_custom_command(
        OUTPUT fp_div_ieee_on_flopoco_${EEE}_${FFF}.cpp
        COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_div_ieee_on_flopoco_EEE_FFF.cpp > fp_div_ieee_on_flopoco_${EEE}_${FFF}.cpp
        DEPENDS fp_div_ieee_on_flopoco_EEE_FFF.cpp
)


endfunction(fp_primitive_add_instance)

foreach(E RANGE 8 11)
    foreach(F RANGE 8 24)
        fp_primitive_add_instance(${E} ${F})
    endforeach(F)

    foreach(F 28 32 36 40 44 48 52)
        fp_primitive_add_instance(${E} ${F})
    endforeach(F)
endforeach(E)


add_executable(fp_primitive_build_main build_main.cpp
    fp_mul_native_8_23.cpp
    fp_mul_native_10_52.cpp
    fp_add_native_8_23.cpp
    fp_add_native_10_52.cpp
    fp_mul_flopoco_8_23.cpp
    fp_add_flopoco_single_8_23.cpp
    fp_add_flopoco_dual_8_23.cpp
        fp_div_flopoco_8_23.cpp
    fp_mul_flopoco_10_52.cpp
    fp_add_flopoco_single_10_52.cpp
    fp_add_flopoco_dual_10_52.cpp
        fp_div_flopoco_10_52.cpp

        fp_add_ieee_on_flopoco_8_23.cpp
        fp_mul_ieee_on_flopoco_8_23.cpp
        fp_div_ieee_on_flopoco_8_23.cpp

        fp_add_ieee_on_flopoco_10_52.cpp
        fp_mul_ieee_on_flopoco_10_52.cpp
        fp_div_ieee_on_flopoco_10_52.cpp
        )

# The vivado top-levels have pragmas on them
target_compile_options(fp_primitive_build_main PUBLIC "-Wno-unknown-pragmas" )
