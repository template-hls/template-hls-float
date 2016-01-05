
file(RELATIVE_PATH THLS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eval_vhls_synth)
add_custom_target(eval_vhls_impl)

function(add_vivado_hls_core_synth DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH ROW_EXTRA)

add_custom_command(OUTPUT build/synth/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS gen_synthesise_tcl.sh
        COMMAND mkdir -p build/synth
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_synthesise_tcl.sh ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} ${THLS_SOURCE_DIR} "" > build/synth/${DESIGN_NAME}_${RATE}_synthesise.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(OUTPUT build/synth/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS all_generated_primitive_cpp
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DESIGN_SRC_PATH}
        WORKING_DIRECTORY build/synth
        COMMAND ${VIVADO_HLS}/bin/vivado_hls -f ${DESIGN_NAME}_${RATE}_synthesise.tcl -l ${DESIGN_NAME}_${RATE}.log
        )

add_custom_command(OUTPUT build/synth/${DESIGN_NAME}_${RATE}.syn.row
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS parse_rpt.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_rpt.py ${ROW_EXTRA}  < build/synth/${DESIGN_NAME}_${RATE}/sim/syn/report/${DESIGN_NAME}_csynth.xml > build/synth/${DESIGN_NAME}_${RATE}.syn.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}.syn.row
        )

add_dependencies(eval_vhls_synth eval_vhls_${DESIGN_NAME}_${RATE}_synth)
        
endfunction(add_vivado_hls_core_synth)


        
function(add_vivado_hls_core_impl DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH MULT ROW_EXTRA)


if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
add_vivado_hls_core_synth( ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} ${ROW_EXTRA})
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

        
add_custom_command(OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        DEPENDS gen_implement_tcl.sh
        COMMAND mkdir -p build/impl
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_implement_tcl.sh ${DESIGN_NAME} ${RATE} ${MULT}  > build/impl/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(
    OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
    OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_synth_utilisation.txt
    OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_opt_utilisation.txt
    OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
    OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
    OUTPUT build/impl/${DESIGN_NAME}_${RATE}_${MULT}_impl_checkpoint.dcp
    DEPENDS build/impl/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_synth
    WORKING_DIRECTORY build/impl
    COMMAND ls .
    COMMAND mkdir -p ${DESIGN_NAME}_${RATE}_${MULT}/log || true
    COMMAND mkdir -p ${DESIGN_NAME}_${RATE}_${MULT}/journal || true
    COMMAND ${VIVADO_ROOT}/bin/vivado -mode batch -log ${DESIGN_NAME}_${RATE}_${MULT}/log -journal ${DESIGN_NAME}_${RATE}_${MULT}/journal -source ${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl | tee ${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
    DEPENDS build/impl/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS build/impl/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_dependencies(eval_vhls_impl
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
)


endfunction(add_vivado_hls_core_impl)


add_custom_command(OUTPUT build/synth_all.row
        DEPENDS eval_vhls_synth
        WORKING_DIRECTORY build
        COMMAND cat synth/*.row > synth_all.row
)
add_custom_target(build_synth_all_row
        DEPENDS build/synth_all.row
)

set(MULTIPLIERS 1.0 1.1 1.2 1.3 1.4 1.5)

foreach(M 100MHz 200MHz 300MHz 400MHz)
        add_vivado_hls_core_synth(fp_add_native_8_23 ${M} fp_add_native_8_23 ../fp/primitive/fp_add_native_8_23.cpp   ,add,native,default,8,23)
        add_vivado_hls_core_synth(fp_mul_native_8_23 ${M} fp_mul_native_8_23 ../fp/primitive/fp_mul_native_8_23.cpp   ,mul,native,default,8,23)
        add_vivado_hls_core_synth(fp_div_native_8_23 ${M} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp   ,div,native,default,8,23)
        
        add_vivado_hls_core_impl(fp_div_native_8_23 ${M} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp  1.0 ,div,native,default,8,23)
        add_vivado_hls_core_impl(fp_div_native_8_23 ${M} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp  1.6 ,div,native,default,8,23)

        foreach(E RANGE 8 8)
            #foreach(F RANGE 8 24)
            foreach(F 8 16 23)
                add_vivado_hls_core_synth(fp_add_flopoco_dual_${E}_${F} ${M} fp_add_flopoco_dual_${E}_${F} ../fp/primitive/fp_add_flopoco_dual_${E}_${F}.cpp  ,add,flopoco_dual,v1,${E},${F})

                add_vivado_hls_core_synth(fp_add_flopoco_single_${E}_${F} ${M} fp_add_flopoco_single_${E}_${F} ../fp/primitive/fp_add_flopoco_single_${E}_${F}.cpp ,add,flopoco_single,v1,${E},${F})

                add_vivado_hls_core_synth(fp_mul_flopoco_${E}_${F} ${M} fp_mul_flopoco_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp ,mul,flopoco,v1,${E},${F})

                add_vivado_hls_core_synth(fp_div_flopoco_${E}_${F} ${M} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp ,div,flopoco,v1,${E},${F})
                add_vivado_hls_core_synth(fp_div_flopoco_v2_${E}_${F} ${M} fp_div_flopoco_v2_${E}_${F} ../fp/primitive/fp_div_flopoco_v2_${E}_${F}.cpp ,div,flopoco,v2,${E},${F})
                #add_vivado_hls_core_synth(fp_div_flopoco_v3_${E}_${F} ${M} fp_div_flopoco_v3_${E}_${F} ../fp/primitive/fp_div_flopoco_v3_${E}_${F}.cpp ,div,flopoco,v3,${E},${F})
                
                
                add_vivado_hls_core_impl(fp_div_flopoco_${E}_${F} ${M} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp 1.0 ,div,flopoco,v1,${E},${F})
            endforeach(F)
        endforeach(E)
endforeach(M)

add_vivado_hls_core_impl(fp_div_flopoco_11_52 200MHz fp_div_flopoco_11_52 ../fp/primitive/fp_div_flopoco_11_52.cpp 1.3 ",div,flopoco,v1,11,52")

