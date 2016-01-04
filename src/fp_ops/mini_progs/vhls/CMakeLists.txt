set(VIVADO_HLS /cygdrive/C/Usr/Xilinx2015.4/Vivado_HLS/2015.4
        CACHE FILEPATH "Location of Vivado HLS root")

set(VIVADO_ROOT /cygdrive/C/Usr/Xilinx2015.4/Vivado/2015.4
        CACHE FILEPATH "Location of Vivado root")

file(RELATIVE_PATH THLS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eval_vhls_synth)

function(add_vivado_hls_core DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH ROW_EXTRA)

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

endfunction(add_vivado_hls_core)


add_custom_command(OUTPUT build/synth_all.row
        DEPENDS eval_vhls_synth
        WORKING_DIRECTORY build
        COMMAND cat synth/*.row > synth_all.row
)
add_custom_target(build_synth_all_row
        DEPENDS build/synth_all.row
)


#foreach(M 50MHz 100MHz 150MHz 200MHz 250MHz 300MHz)
foreach(M 100MHz 200MHz 300MHz)
        add_vivado_hls_core(fp_add_native_8_23 ${M} fp_add_native_8_23 ../fp/primitive/fp_add_native_8_23.cpp   ,add,native,default,8,23)
        add_vivado_hls_core(fp_mul_native_8_23 ${M} fp_mul_native_8_23 ../fp/primitive/fp_mul_native_8_23.cpp   ,mul,native,default,8,23)
        add_vivado_hls_core(fp_div_native_8_23 ${M} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp   ,div,native,default,8,23)

        foreach(E RANGE 8 8)
            #foreach(F RANGE 8 24)
            foreach(F RANGE 8 12 16 20 23)
                add_vivado_hls_core(fp_add_flopoco_dual_${E}_${F} ${M} fp_add_flopoco_dual_${E}_${F} ../fp/primitive/fp_add_flopoco_dual_${E}_${F}.cpp  ,add,flopoco_dual,v1,${E},${F})

                add_vivado_hls_core(fp_add_flopoco_single_${E}_${F} ${M} fp_add_flopoco_single_${E}_${F} ../fp/primitive/fp_add_flopoco_single_${E}_${F}.cpp ,add,flopoco_single,v1,${E},${F})

                add_vivado_hls_core(fp_mul_flopoco_${E}_${F} ${M} fp_mul_flopoco_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp ,mul,flopoco,v1,${E},${F})

                add_vivado_hls_core(fp_div_flopoco_${E}_${F} ${M} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp ,div,flopoco,v1,${E},${F})
                add_vivado_hls_core(fp_div_flopoco_v2_${E}_${F} ${M} fp_div_flopoco_v2_${E}_${F} ../fp/primitive/fp_div_flopoco_v2_${E}_${F}.cpp ,div,flopoco,v2,${E},${F})
                #add_vivado_hls_core(fp_div_flopoco_v3_${E}_${F} ${M} fp_div_flopoco_v3_${E}_${F} ../fp/primitive/fp_div_flopoco_v3_${E}_${F}.cpp ,div,flopoco,v3,${E},${F})
            endforeach(F)
        endforeach(E)
endforeach(M)
