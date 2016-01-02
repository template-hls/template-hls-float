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

add_executable_test_with_fw_uint(base test_add_dual)

add_executable_test_with_fw_uint(base test_lut_opt)

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

add_subdirectory(mini_progs)
