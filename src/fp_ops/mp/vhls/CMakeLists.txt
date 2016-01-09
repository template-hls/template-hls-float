# The build/synth/ -> b/i/ change is because of path length limitations


file(RELATIVE_PATH THLS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eval_vhls_synth)
add_custom_target(eval_vhls_impl)

function(add_vivado_hls_core_synth DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH ROW_EXTRA)

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS gen_synthesise_tcl.sh
        COMMAND mkdir -p b/s
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_synthesise_tcl.sh ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} ${THLS_SOURCE_DIR} "" > b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS all_generated_primitive_cpp
        DEPENDS b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DESIGN_SRC_PATH}
        WORKING_DIRECTORY b/s
        COMMAND ${VIVADO_HLS}/bin/vivado_hls -f ${DESIGN_NAME}_${RATE}_synthesise.tcl -l ${DESIGN_NAME}_${RATE}.log
        )

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}.syn.row
        DEPENDS b/s/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS parse_rpt.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_rpt.py ${ROW_EXTRA}  < b/s/${DESIGN_NAME}_${RATE}/sim/syn/report/${DESIGN_NAME}_csynth.xml > b/s/${DESIGN_NAME}_${RATE}.syn.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth
        DEPENDS b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS b/s/${DESIGN_NAME}_${RATE}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS b/s/${DESIGN_NAME}_${RATE}.syn.row
        )

add_dependencies(eval_vhls_synth eval_vhls_${DESIGN_NAME}_${RATE}_synth)
        
endfunction(add_vivado_hls_core_synth)


        
function(add_vivado_hls_core_impl DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH MULT ROW_EXTRA)


if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
add_vivado_hls_core_synth( ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} ${ROW_EXTRA})
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

        
add_custom_command(OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        DEPENDS gen_implement_tcl.sh
        COMMAND mkdir -p b/i/
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_implement_tcl.sh ${DESIGN_NAME} ${RATE} ${MULT}  > b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_synth_utilisation.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_opt_utilisation.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
    OUTPUT b/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_checkpoint.dcp
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_synth
    WORKING_DIRECTORY b/i/
    COMMAND ls .
    COMMAND mkdir -p ${DESIGN_NAME}_${RATE}_${MULT}/log || true
    COMMAND mkdir -p ${DESIGN_NAME}_${RATE}_${MULT}/journal || true
    COMMAND ${VIVADO_ROOT}/bin/vivado -mode batch -log ${DESIGN_NAME}_${RATE}_${MULT}/log -journal ${DESIGN_NAME}_${RATE}_${MULT}/journal -source ${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl | tee ${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS b/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_dependencies(eval_vhls_impl
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
)


endfunction(add_vivado_hls_core_impl)


add_custom_command(OUTPUT b/synth_all.row
        DEPENDS eval_vhls_synth
        WORKING_DIRECTORY build
        COMMAND cat synth/*.row > synth_all.row
)
add_custom_target(build_synth_all_row
        DEPENDS b/synth_all.row
)

set(MULTIPLIERS 1.0 1.1 1.2 1.3 1.4 1.5)

########################################################################################
## Native

### Synth

function(add_all_native_single_core_synth  RATE)
add_vivado_hls_core_synth(fp_add_native_8_23 ${RATE} fp_add_native_8_23 ../fp/primitive/fp_add_native_8_23.cpp   ,add,native,default,8,23)
add_vivado_hls_core_synth(fp_add_native_nodsp_8_23 ${RATE} fp_add_native_nodsp_8_23 ../fp/primitive/fp_add_native_nodsp_8_23.cpp   ,add,native,nodsp,8,23)
add_vivado_hls_core_synth(fp_add_native_fulldsp_8_23 ${RATE} fp_add_native_fulldsp_8_23 ../fp/primitive/fp_add_native_fulldsp_8_23.cpp   ,add,native,fulldsp,8,23)

add_vivado_hls_core_synth(fp_mul_native_8_23 ${RATE} fp_mul_native_8_23 ../fp/primitive/fp_mul_native_8_23.cpp   ,mul,native,default,8,23)
add_vivado_hls_core_synth(fp_mul_native_nodsp_8_23 ${RATE} fp_mul_native_nodsp_8_23 ../fp/primitive/fp_mul_native_nodsp_8_23.cpp   ,mul,native,nodsp,8,23)
add_vivado_hls_core_synth(fp_mul_native_meddsp_8_23 ${RATE} fp_mul_native_meddsp_8_23 ../fp/primitive/fp_mul_native_meddsp_8_23.cpp   ,mul,native,meddsp,8,23)
add_vivado_hls_core_synth(fp_mul_native_fulldsp_8_23 ${RATE} fp_mul_native_fulldsp_8_23 ../fp/primitive/fp_mul_native_fulldsp_8_23.cpp   ,mul,native,fulldsp,8,23)
add_vivado_hls_core_synth(fp_mul_native_maxdsp_8_23 ${RATE} fp_mul_native_maxdsp_8_23 ../fp/primitive/fp_mul_native_maxdsp_8_23.cpp   ,mul,native,maxdsp,8,23)


add_vivado_hls_core_synth(fp_div_native_8_23 ${RATE} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp   ,div,native,default,8,23)
endfunction(add_all_native_single_core_synth)

function(add_all_native_double_core_synth  RATE)
add_vivado_hls_core_synth(fp_add_native_11_52 ${RATE} fp_add_native_11_52 ../fp/primitive/fp_add_native_11_52.cpp   ,add,native,default,11,52)
add_vivado_hls_core_synth(fp_mul_native_11_52 ${RATE} fp_mul_native_11_52 ../fp/primitive/fp_mul_native_11_52.cpp   ,mul,native,default,11,52)
add_vivado_hls_core_synth(fp_div_native_11_52 ${RATE} fp_div_native_11_52 ../fp/primitive/fp_div_native_11_52.cpp   ,div,native,default,11,52)
endfunction(add_all_native_double_core_synth)

### Impl

function(add_all_native_single_core_impl  RATE MULT)
add_vivado_hls_core_impl(fp_add_native_8_23 ${RATE} fp_add_native_8_23 ../fp/primitive/fp_add_native_8_23.cpp  ${MULT} ,add,native,default,8,23)
add_vivado_hls_core_impl(fp_add_native_nodsp_8_23 ${RATE} fp_add_native_nodsp_8_23 ../fp/primitive/fp_add_native_nodsp_8_23.cpp  ${MULT} ,add,native,nodsp,8,23)
add_vivado_hls_core_impl(fp_add_native_fulldsp_8_23 ${RATE} fp_add_native_fulldsp_8_23 ../fp/primitive/fp_add_native_fulldsp_8_23.cpp  ${MULT} ,add,native,fulldsp,8,23)

add_vivado_hls_core_impl(fp_mul_native_8_23 ${RATE} fp_mul_native_8_23 ../fp/primitive/fp_mul_native_8_23.cpp  ${MULT} ,mul,native,default,8,23)
add_vivado_hls_core_impl(fp_mul_native_nodsp_8_23 ${RATE} fp_mul_native_nodsp_8_23 ../fp/primitive/fp_mul_native_nodsp_8_23.cpp  ${MULT} ,mul,native,nodsp,8,23)
add_vivado_hls_core_impl(fp_mul_native_meddsp_8_23 ${RATE} fp_mul_native_meddsp_8_23 ../fp/primitive/fp_mul_native_meddsp_8_23.cpp  ${MULT} ,mul,native,meddsp,8,23)
add_vivado_hls_core_impl(fp_mul_native_fulldsp_8_23 ${RATE} fp_mul_native_fulldsp_8_23 ../fp/primitive/fp_mul_native_fulldsp_8_23.cpp  ${MULT} ,mul,native,fulldsp,8,23)
add_vivado_hls_core_impl(fp_mul_native_maxdsp_8_23 ${RATE} fp_mul_native_maxdsp_8_23 ../fp/primitive/fp_mul_native_maxdsp_8_23.cpp  ${MULT} ,mul,native,maxdsp,8,23)

add_vivado_hls_core_impl(fp_div_native_8_23 ${RATE} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp  ${MULT} ,div,native,default,8,23)
endfunction(add_all_native_single_core_impl)

function(add_all_native_double_core_impl  RATE MULT)
add_vivado_hls_core_impl(fp_add_native_11_52 ${RATE} fp_add_native_11_52 ../fp/primitive/fp_add_native_11_52.cpp  ${MULT} ,add,native,default,11,52)
add_vivado_hls_core_impl(fp_mul_native_11_52 ${RATE} fp_mul_native_11_52 ../fp/primitive/fp_mul_native_11_52.cpp  ${MULT} ,mul,native,default,11,52)
add_vivado_hls_core_impl(fp_div_native_11_52 ${RATE} fp_div_native_11_52 ../fp/primitive/fp_div_native_11_52.cpp  ${MULT} ,div,native,default,11,52)
endfunction(add_all_native_double_core_impl)

######################################################
## Flopoco

function(add_all_flopoco_core_synth E F RATE)
add_vivado_hls_core_synth(fp_add_flopoco_dual_${E}_${F} ${RATE} fp_add_flopoco_dual_${E}_${F} ../fp/primitive/fp_add_flopoco_dual_${E}_${F}.cpp  ,add,flopoco_dual,v1,${E},${F})

add_vivado_hls_core_synth(fp_add_flopoco_single_${E}_${F} ${RATE} fp_add_flopoco_single_${E}_${F} ../fp/primitive/fp_add_flopoco_single_${E}_${F}.cpp  ,add,flopoco_single,v1,${E},${F})
add_vivado_hls_core_synth(fp_add_flopoco_single_v2_${E}_${F} ${RATE} fp_add_flopoco_single_v2_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v2_${E}_${F}.cpp  ,add,flopoco_single,v2,${E},${F})
add_vivado_hls_core_synth(fp_add_flopoco_single_v3_${E}_${F} ${RATE} fp_add_flopoco_single_v3_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v3_${E}_${F}.cpp  ,add,flopoco_single,v3,${E},${F})
add_vivado_hls_core_synth(fp_add_flopoco_single_v4_${E}_${F} ${RATE} fp_add_flopoco_single_v4_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v4_${E}_${F}.cpp  ,add,flopoco_single,v4,${E},${F})

add_vivado_hls_core_synth(fp_mul_flopoco_${E}_${F} ${RATE} fp_mul_flopoco_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp ,mul,flopoco,v1,${E},${F})

add_vivado_hls_core_synth(fp_div_flopoco_${E}_${F} ${RATE} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp ,div,flopoco,v1,${E},${F})
add_vivado_hls_core_synth(fp_div_flopoco_v2_${E}_${F} ${RATE} fp_div_flopoco_v2_${E}_${F} ../fp/primitive/fp_div_flopoco_v2_${E}_${F}.cpp ,div,flopoco,v2,${E},${F})
#add_vivado_hls_core_synth(fp_div_flopoco_v3_${E}_${F} ${RATE} fp_div_flopoco_v3_${E}_${F} ../fp/primitive/fp_div_flopoco_v3_${E}_${F}.cpp ,div,flopoco,v3,${E},${F})
endfunction(add_all_flopoco_core_synth)

function(add_all_flopoco_core_impl E F RATE MULT)
add_vivado_hls_core_impl(fp_add_flopoco_dual_${E}_${F} ${RATE} fp_add_flopoco_dual_${E}_${F} ../fp/primitive/fp_add_flopoco_dual_${E}_${F}.cpp  ${MULT}  ,add,flopoco_dual,v1,${E},${F})

add_vivado_hls_core_impl(fp_add_flopoco_single_${E}_${F} ${RATE} fp_add_flopoco_single_${E}_${F} ../fp/primitive/fp_add_flopoco_single_${E}_${F}.cpp  ${MULT}  ,add,flopoco_single,v1,${E},${F})
add_vivado_hls_core_impl(fp_add_flopoco_single_v2_${E}_${F} ${RATE} fp_add_flopoco_single_v2_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v2_${E}_${F}.cpp  ${MULT}  ,add,flopoco_single,v2,${E},${F})
add_vivado_hls_core_impl(fp_add_flopoco_single_v3_${E}_${F} ${RATE} fp_add_flopoco_single_v3_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v3_${E}_${F}.cpp  ${MULT}  ,add,flopoco_single,v3,${E},${F})
add_vivado_hls_core_impl(fp_add_flopoco_single_v4_${E}_${F} ${RATE} fp_add_flopoco_single_v4_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v4_${E}_${F}.cpp  ${MULT}  ,add,flopoco_single,v4,${E},${F})

add_vivado_hls_core_impl(fp_mul_flopoco_${E}_${F} ${RATE} fp_mul_flopoco_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp ${MULT} ,mul,flopoco,v1,${E},${F})

add_vivado_hls_core_impl(fp_div_flopoco_${E}_${F} ${RATE} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp  ${MULT} ,div,flopoco,v1,${E},${F})
add_vivado_hls_core_impl(fp_div_flopoco_v2_${E}_${F} ${RATE} fp_div_flopoco_v2_${E}_${F} ../fp/primitive/fp_div_flopoco_v2_${E}_${F}.cpp ${MULT} ,div,flopoco,v2,${E},${F})
#add_vivado_hls_core_impl(fp_div_flopoco_v3_${E}_${F} ${RATE} fp_div_flopoco_v3_${E}_${F} ../fp/primitive/fp_div_flopoco_v3_${E}_${F}.cpp ${MULT} ,div,flopoco,v3,${E},${F})
endfunction(add_all_flopoco_core_impl)



foreach(M 100MHz 200MHz)
        add_all_native_single_core_impl(${M} 1.0)
        #add_all_native_single_core_impl(${M} 1.1)
        add_all_native_single_core_impl(${M} 1.2)
        
        add_all_flopoco_core_impl(5 10 ${M} 1.0)
        #add_all_flopoco_core_impl(5 10 ${M} 1.1)
        add_all_flopoco_core_impl(5 10 ${M} 1.2)
        
        add_all_flopoco_core_impl(7 15 ${M} 1.0)
        #add_all_flopoco_core_impl(7 15 ${M} 1.1)
        add_all_flopoco_core_impl(7 15 ${M} 1.2)
        
        add_all_flopoco_core_impl(8 23 ${M} 1.0)
        #add_all_flopoco_core_impl(8 23 ${M} 1.1)
        add_all_flopoco_core_impl(8 23 ${M} 1.2)
endforeach(M)


