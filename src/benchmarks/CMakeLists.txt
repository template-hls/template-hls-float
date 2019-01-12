string(REPLACE "/cygdrive/e" "e:/" THLS_SOURCE_DIR "${CMAKE_SOURCE_DIR}" )


function(add_vhls_core_synth TARGET DESIGN_NAME RATE TOPLEVEL_NAME  DESIGN_SRC_PATH ROW_EXTRA)

get_filename_component(ABS_DESIGN_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${DESIGN_SRC_PATH} ABSOLUTE)
string(REPLACE "/cygdrive/e" "e:/" ABS_DESIGN_SRC_PATH "${ABS_DESIGN_SRC_PATH}" )

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS gen_synthesise_tcl.sh
        COMMAND mkdir -p b/s
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_synthesise_tcl.sh ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${SYNTH_PROJ_NAME} ${ABS_DESIGN_SRC_PATH} ${THLS_SOURCE_DIR} "" > b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(OUTPUT b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${TOPLEVEL_NAME}.vhd
        DEPENDS b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${DESIGN_SRC_PATH}
        WORKING_DIRECTORY b/s
        COMMAND ${VIVADO_HLS}/bin/vivado_hls -f ${DESIGN_NAME}_${RATE}_synthesise.tcl -l ${DESIGN_NAME}_${RATE}.log
        COMMAND rm -rf ${DESIGN_NAME}_${RATE}/sim/.autopilot
        )

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}.syn.row
        DEPENDS b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${TOPLEVEL_NAME}.vhd
        DEPENDS parse_rpt.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_rpt.py ${ROW_EXTRA}  < b/s/${SYNTH_PROJ_NAME}/sim/syn/report/${TOPLEVEL_NAME}_csynth.xml > b/s/${DESIGN_NAME}_${RATE}.syn.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth
        DEPENDS b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${TOPLEVEL_NAME}.vhd
        DEPENDS b/s/${DESIGN_NAME}_${RATE}.syn.row
        )

add_dependencies(${TARGET} eval_vhls_${DESIGN_NAME}_${RATE}_synth)

endfunction(add_vhls_core_synth)



function(add_vhls_core_impl TARGET DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH MULT ROW_EXTRA)


if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
add_vhls_core_synth( ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} ${ROW_EXTRA})
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

string(MD5 IMPL_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${IMPL_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)


add_custom_command(OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        DEPENDS gen_implement_tcl.sh
        COMMAND mkdir -p b/i/
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_implement_tcl.sh ${DESIGN_NAME} ${SYNTH_PROJ_NAME} ${IMPL_PROJ_NAME} ${RATE} ${MULT}  > b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_synth_utilisation.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_opt_utilisation.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
    #OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_checkpoint.dcp
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_synth
    WORKING_DIRECTORY b/i/
    COMMAND mkdir -p ${IMPL_PROJ_NAME}_impl/log || true
    COMMAND mkdir -p ${IMPL_PROJ_NAME}_impl/journal || true
    COMMAND ${VIVADO_ROOT}/bin/vivado -mode batch -log ${IMPL_PROJ_NAME}_impl/log/log.txt -journal ${IMPL_PROJ_NAME}_impl/journal/journal.txt -source ${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl | tee ${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_custom_command(OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
        DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
        DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
        DEPENDS parse_impl_logs.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_impl_logs.py b/i/${DESIGN_NAME}_${RATE}_${MULT} ${DESIGN_NAME}_${RATE}_${MULT}  ${IMPL_PROJ_NAME} "  ,${MULT}  ${ROW_EXTRA}" > b/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_row
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_dependencies(${TARGET}
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
)

endfunction(add_vhls_core_impl)



add_custom_target(eval_benchmark_dot_par4_synth)
add_custom_target(eval_benchmark_dot_par4_impl)

function(add_benchmark_dot_par4 VARIANT RATE)
string(REPLACE "_" "," ROW_EXTRA "${VARIANT}" )
set( ROW_EXTRA ",${ROW_EXTRA}")

add_vhls_core_synth(eval_benchmark_dot_par4_synth  dot_product_par4_tl_${VARIANT}  ${RATE}   dot_product_par4 dot_product/dot_product_par4_tl_${VARIANT}.cpp       ${ROW_EXTRA})
add_vhls_core_impl( eval_benchmark_dot_par4_impl  dot_product_par4_tl_${VARIANT}   ${RATE}   dot_product_par4 dot_product/dot_product_par4_tl_${VARIANT}.cpp  1.0  ${ROW_EXTRA})
endfunction(add_benchmark_dot_par4)


foreach( RATE 100MHz 200MHz 300MHz)

add_benchmark_dot_par4(hls16_hls16_hls16_round ${RATE})
add_benchmark_dot_par4(hls24_hls16_hls16_round ${RATE})
add_benchmark_dot_par4(hls24_hls24_hls24_round ${RATE})
add_benchmark_dot_par4(hls32_hls16_hls16_round ${RATE})
add_benchmark_dot_par4(hls32_hls16_hls16_full ${RATE})
add_benchmark_dot_par4(hls32_hls32_hls16_round ${RATE})
add_benchmark_dot_par4(hls32_hls32_hls32_round ${RATE})
add_benchmark_dot_par4(hls48_hls24_hls24_round ${RATE})
add_benchmark_dot_par4(hls48_hls48_hls48_round ${RATE})
add_benchmark_dot_par4(hls64_hls32_hls32_round ${RATE})
add_benchmark_dot_par4(hls64_hls32_hls32_full ${RATE})
add_benchmark_dot_par4(hls64_hls64_hls32_round ${RATE})
add_benchmark_dot_par4(hls64_hls64_hls64_round ${RATE})

add_benchmark_dot_par4(nat16_nat16_nat16_round ${RATE})
add_benchmark_dot_par4(nat32_nat16_nat16_round ${RATE})
add_benchmark_dot_par4(nat32_nat32_nat16_round ${RATE})
add_benchmark_dot_par4(nat32_nat32_nat32_round ${RATE})
add_benchmark_dot_par4(nat64_nat32_nat32_round ${RATE})
add_benchmark_dot_par4(nat64_nat64_nat32_round ${RATE})
add_benchmark_dot_par4(nat64_nat64_nat64_round ${RATE})

add_benchmark_dot_par4(soft32_soft32_soft32_round ${RATE})

endforeach(RATE)


add_custom_target(eval_benchmark_mat_mul_synth)
add_custom_target(eval_benchmark_mat_mul_impl)

function(add_benchmark_mat_mul VARIANT RATE)
string(REPLACE "_" "," ROW_EXTRA "${VARIANT}" )
set( ROW_EXTRA ",${ROW_EXTRA}")

add_vhls_core_synth(eval_benchmark_mat_mul_synth  mat_mul_16x16_${VARIANT}  ${RATE}   mat_mul mat_mul/mat_mul_16x16_${VARIANT}.cpp       ${ROW_EXTRA})
add_vhls_core_impl( eval_benchmark_mat_mul_impl   mat_mul_16x16_${VARIANT}   ${RATE}   mat_mul mat_mul/mat_mul_16x16_${VARIANT}.cpp  1.0  ${ROW_EXTRA})
endfunction(add_benchmark_mat_mul)


foreach( RATE 100MHz)

add_benchmark_mat_mul(hls16_hls16_hls16_round ${RATE})

endforeach(RATE)