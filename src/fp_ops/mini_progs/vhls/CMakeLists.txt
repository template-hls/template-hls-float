set(VIVADO_HLS /cygdrive/C/Usr/Xilix2015.4/Vivado_HLS/2015.4
        CACHE FILEPATH "Location of Vivado HLS root")

set(VIVADO_ROOT /cygdrive/C/Usr/Xilix2015.4/Vivado/2015.4
        CACHE FILEPATH "Location of Vivado root")

file(RELATIVE_PATH THLS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eval_vhls_synth)

function(add_vivado_hls_core DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH)

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
        COMMAND ${VIVADO_HLS}/bin/vivado_hls  -f ${DESIGN_NAME}_${RATE}_synthesise.tcl
        )

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS build/synth/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        )

add_dependencies(eval_vhls_synth eval_vhls_${DESIGN_NAME}_${RATE}_synth)

endfunction(add_vivado_hls_core)


foreach(M 50MHz 100MHz 150MHz 200MHz 250MHz 300MHz)
        add_vivado_hls_core(fp_primitive_add_native_8_23 ${M} fp_add_native_8_23 ../fp/primitive/fp_add_native_8_23.cpp)
        add_vivado_hls_core(fp_primitive_mul_native_8_23 ${M} fp_mul_native_8_23 ../fp/primitive/fp_mul_native_8_23.cpp)
        add_vivado_hls_core(fp_primitive_div_native_8_23 ${M} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp)

        foreach(E RANGE 8 8)
            foreach(F RANGE 8 32)
                add_vivado_hls_core(fp_primitive_add_flopoco_dual_${E}_${F} ${M} fp_add_flopoco_dual_${E}_${F} ../fp/primitive/fp_add_flopoco_dual_${E}_${F}.cpp)
                add_vivado_hls_core(fp_primitive_add_flopoco_single_${E}_${F} ${M} fp_add_flopoco_single_${E}_${F} ../fp/primitive/fp_add_flopoco_single_${E}_${F}.cpp)
                add_vivado_hls_core(fp_primitive_div_flopoco_${E}_${F} ${M} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp)
                add_vivado_hls_core(fp_primitive_mul_flopoco_${E}_${F} ${M} fp_mul_flopoco_dual_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp)
            endforeach(F)
        endforeach(E)
endforeach(M)
