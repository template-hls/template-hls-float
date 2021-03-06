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
#add_executable_test_with_fw_uint(base test_dot2_pos_rndn)
add_executable_test_with_fw_uint(base test_dot4_pos_rndd)

add_custom_target(build_all_cmult_impl)
add_custom_target(build_all_cmult_impl_long)


add_custom_target(test_add_param_run)

function(add_param_test E F )

set(__E__ ${E})
set(__F__ ${F})

configure_file(test_add_param.cpp test_add_param_${E}_${F}.cpp  )
add_executable_test_with_fw_uint(base test_add_param_${E}_${F})

configure_file(test_mul_param.cpp test_mul_param_${E}_${F}.cpp  )
add_executable_test_with_fw_uint(base test_mul_param_${E}_${F})

configure_file(test_div_param.cpp test_div_param_${E}_${F}.cpp  )
add_executable_test_with_fw_uint(base test_div_param_${E}_${F})


#target_link_libraries(test_add_param_${E}_${F} ${MPFR_LIBRARY} ${GMP_LIBRARIES})
#target_compile_definitions(test_add_param_${E}_${F} PUBLIC "-D__E__=${E}" "-D__F__=${F}" )

#add_custom_command(
#    OUTPUT test_add_param_${E}_${F}.row test_add_param_${E}_${F}.input
#    COMMAND test_add_param_${E}_${F} test_add_param_${E}_${F}.input > test_add_param_${E}_${F}.row
#    DEPENDS test_add_param_${E}_${F}
#)

#add_custom_target(test_add_param_${E}_${F}_run
#    DEPENDS test_add_param_${E}_${F}.input test_add_param_${E}_${F}.row
#)
#add_dependencies(test_add_param_run test_add_param_${E}_${F}_run) 

endfunction(add_param_test)

foreach(E RANGE 4 12)
    foreach(F RANGE 3 32)
        add_param_test(${E} ${F})
    endforeach(F)
endforeach(E)

#add_subdirectory(mp)
