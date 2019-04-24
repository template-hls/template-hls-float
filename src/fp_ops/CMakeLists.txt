include_directories( ${MPFR_INCLUDE_DIR} )

add_executable_test_with_fw_uint(base test_bits)
add_executable_test_with_fw_uint(base test_fp_flopoco)
add_executable_test_with_fw_uint(base test_fp_ieee)

add_executable_test_with_fw_uint(base test_bit_heap)
add_executable_test_with_fw_uint(base test_row_heap)

add_executable_test_with_fw_uint(base test_mul)
add_executable_test_with_fw_uint(base test_mul_different_frac)

add_executable_test_with_fw_uint(base test_add_single)
add_executable_test_with_fw_uint(base test_add_single_different_frac)
add_executable_test_with_fw_uint(base test_add_single_different_exp)
add_executable_test_with_fw_uint(base test_add_single_debug)

add_executable_test_with_fw_uint(base test_add_single_v2)
add_executable_test_with_fw_uint(base test_add_single_v3)
add_executable_test_with_fw_uint(base test_add_single_v4)

add_executable_test_with_fw_uint(base test_add_single_different_frac_v5)

#add_executable_test_with_fw_uint(base test_fp2fix)

add_executable_test_with_fw_uint(base test_div)
add_executable_test_with_fw_uint(base test_div_v2)
add_executable_test_with_fw_uint(base test_div_v2_debug)
#add_executable_test_with_fw_uint(base test_div_v3)

add_executable_test_with_fw_uint(base test_inv_nr)

add_executable_test_with_fw_uint(base test_policy)
add_executable_test_with_fw_uint(base test_policy_native)
add_executable_test_with_fw_uint(base test_policy_flopoco)
add_executable_test_with_fw_uint(base test_policy_ieee_on_flopoco)
add_executable_test_with_fw_uint(base test_policy_softfloat)

add_executable_test_with_fw_uint(base test_add_dual)
add_executable_test_with_fw_uint(base test_add_dual_debug)

add_executable_test_with_fw_uint(base test_lut_opt)


add_executable_test_with_fw_uint(base test_fp_convert)
add_executable_test_with_fw_uint(base test_fma)
add_executable_test_with_fw_uint(base test_dot2_pos_rndd)
add_executable_test_with_fw_uint(base test_dot2_pos_rndn)
add_executable_test_with_fw_uint(base test_dot4_pos_rndd)

add_custom_target(build_all_cmult_impl)
add_custom_target(build_all_cmult_impl_long)

function(add_cmult_impl NAME)
set(__IMPL__ cmult_impl_${NAME})
set(THLS_CMULT_LONG_TEST 0)
set(THLS_CMULT_TINY_TEST 1)

configure_file(test_cmult_generic.cpp.in test_cmult_${NAME}_tiny.cpp)
add_executable_test_with_fw_uint(base test_cmult_${NAME}_tiny)
add_dependencies(build_all_cmult_impl test_cmult_${NAME}_tiny)

set(THLS_CMULT_TINY_TEST 0)
configure_file(test_cmult_generic.cpp.in test_cmult_${NAME}.cpp)
add_executable_test_with_fw_uint(base test_cmult_${NAME})
add_dependencies(build_all_cmult_impl test_cmult_${NAME})

set(THLS_CMULT_LONG_TEST 1)
configure_file(test_cmult_generic.cpp.in test_cmult_${NAME}_long.cpp)
add_executable_test_with_fw_uint(base test_cmult_${NAME}_long)
add_dependencies(build_all_cmult_impl test_cmult_${NAME}_long)

configure_file(test_cmult_perf.cpp.in test_cmult_perf_${NAME}.cpp)
add_executable_test_with_fw_uint(base test_cmult_perf_${NAME})
add_dependencies(build_all_cmult_impl test_cmult_perf_${NAME})

endfunction(add_cmult_impl)

add_cmult_impl(v1_native)
add_cmult_impl(v2_unrolled_full)
add_cmult_impl(v3_unrolled_masked)
add_cmult_impl(v4_recursive_masked)
add_cmult_impl(v5_booth)
add_cmult_impl(v6_booth)
add_cmult_impl(v7_kcm_k4)
add_cmult_impl(v7_kcm_k5)
add_cmult_impl(v8_kcm_k2)
add_cmult_impl(v8_kcm_k4)
add_cmult_impl(v8_kcm_k5)
add_cmult_impl(v9_kcm_k1)
add_cmult_impl(v9_kcm_k2)
add_cmult_impl(v10_booth_cache)


add_custom_target(test_add_param_run)

function(add_param_test E F )

add_executable(test_add_param_${E}_${F} test_add_param.cpp)
target_link_libraries(test_add_param_${E}_${F} ${MPFR_LIBRARY} ${GMP_LIBRARIES})
target_compile_definitions(test_add_param_${E}_${F} PUBLIC "-D__E__=${E}" "-D__F__=${F}" )

add_custom_command(
    OUTPUT test_add_param_${E}_${F}.row test_add_param_${E}_${F}.input
    COMMAND test_add_param_${E}_${F} test_add_param_${E}_${F}.input > test_add_param_${E}_${F}.row
    DEPENDS test_add_param_${E}_${F}
)

add_custom_target(test_add_param_${E}_${F}_run
    DEPENDS test_add_param_${E}_${F}.input test_add_param_${E}_${F}.row
)
add_dependencies(test_add_param_run test_add_param_${E}_${F}_run)

endfunction(add_param_test)

#foreach(E RANGE 8 8)
#    foreach(F RANGE 8 32)
#        add_param_test(${E} ${F})
#    endforeach(F)
#endforeach(E)

#add_subdirectory(mp)
