

function(fp_primitive_add_instance EEE FFF )

add_custom_command(
    OUTPUT fp_mul_flopoco_${EEE}_${FFF}.cpp
    COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_mul_flopoco_EEE_FFF.cpp > fp_mul_flopoco_${EEE}_${FFF}.cpp
    DEPENDS fp_mul_flopoco_EEE_FFF.cpp
)

add_custom_command(
    OUTPUT fp_add_flopoco_single_${EEE}_${FFF}.cpp
    COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_add_flopoco_single_EEE_FFF.cpp > fp_add_flopoco_single_${EEE}_${FFF}.cpp
    DEPENDS fp_add_flopoco_single_EEE_FFF.cpp
)

add_custom_command(
    OUTPUT fp_add_flopoco_dual_${EEE}_${FFF}.cpp
    COMMAND sed -r -e "s/EEE/${EEE}/g" -e "s/FFF/${FFF}/g" ${CMAKE_CURRENT_SOURCE_DIR}/fp_add_flopoco_dual_EEE_FFF.cpp > fp_add_flopoco_dual_${EEE}_${FFF}.cpp
    DEPENDS fp_add_flopoco_dual_EEE_FFF.cpp
)

endfunction(fp_primitive_add_instance)

fp_primitive_add_instance(8 23)
fp_primitive_add_instance(10 52)

add_executable(fp_primitive_build_main build_main.cpp
    fp_mul_native_8_23.cpp
    fp_mul_native_10_52.cpp
    fp_add_native_8_23.cpp
    fp_add_native_10_52.cpp
    fp_mul_flopoco_8_23.cpp
    fp_add_flopoco_single_8_23.cpp
    fp_add_flopoco_dual_8_23.cpp
    fp_mul_flopoco_10_52.cpp
    fp_add_flopoco_single_10_52.cpp
    fp_add_flopoco_dual_10_52.cpp
)

# The vivado top-levels have pragmas on them
target_compile_options(fp_primitive_build_main PUBLIC "-Wno-unknown-pragmas" )
