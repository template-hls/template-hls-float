# The build/synth/ -> b/i/ change is because of path length limitations


file(RELATIVE_PATH THLS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

find_program(FLOPOCO flopoco HINTS /cygdrive/e/external/flopoco/trunk/gcc ${CMAKE_SOURCE_DIR}/bin)


function(add_vivado_hls_core_synth GROUP DESIGN_NAME RATE TOPLEVEL_NAME  DESIGN_SRC_PATH ROW_EXTRA HDR_EXTRA)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

add_custom_command(OUTPUT ${GROUP}/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS gen_synthesise_tcl.sh
        COMMAND mkdir -p ${GROUP}/s
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_synthesise_tcl.sh ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${SYNTH_PROJ_NAME} ${DESIGN_SRC_PATH} ${THLS_SOURCE_DIR} "" > ${GROUP}/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        WORKING_DIRECTORY .
        )

add_custom_target(${GROUP}_s_${DESIGN_NAME}_${RATE}_synthesise_tcl
        DEPENDS ${GROUP}/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
)

add_custom_command(OUTPUT ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${TOPLEVEL_NAME}.vhd
        DEPENDS all_generated_primitive_cpp
        DEPENDS ${GROUP}_s_${DESIGN_NAME}_${RATE}_synthesise_tcl
        DEPENDS ${GROUP}/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DESIGN_SRC_PATH}
        WORKING_DIRECTORY ${GROUP}/s
        COMMAND ${VIVADO_HLS}/bin/vivado_hls -f ${DESIGN_NAME}_${RATE}_synthesise.tcl -l ${DESIGN_NAME}_${RATE}.log
        COMMAND rm -rf ${DESIGN_NAME}_${RATE}/sim/.autopilot
        )

add_custom_target(${GROUP}_s_${SYNTH_PROJ_NAME}_sim_syn_vhdl_${TOPLEVEL_NAME}_vhd
        DEPENDS ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${TOPLEVEL_NAME}.vhd
)

add_custom_command(OUTPUT ${GROUP}/s/${DESIGN_NAME}_${RATE}.syn.row
        DEPENDS ${GROUP}_s_${SYNTH_PROJ_NAME}_sim_syn_vhdl_${TOPLEVEL_NAME}_vhd
        DEPENDS ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${TOPLEVEL_NAME}.vhd
        DEPENDS parse_rpt.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_rpt.py ${DESIGN_NAME}_${RATE}_${GROUP} ${GROUP} "${ROW_EXTRA}" "${HDR_EXTRA}"  < ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/report/${TOPLEVEL_NAME}_csynth.xml > ${GROUP}/s/${DESIGN_NAME}_${RATE}.syn.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_synth_row_${GROUP}
    DEPENDS ${GROUP}/s/${DESIGN_NAME}_${RATE}.syn.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
        DEPENDS ${GROUP}_s_${DESIGN_NAME}_${RATE}_synthesise_tcl
        DEPENDS ${GROUP}_s_${SYNTH_PROJ_NAME}_sim_syn_vhdl_${TOPLEVEL_NAME}_vhd
        DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_synth_row_${GROUP}
        )

add_dependencies(eval_vhls_synth_${GROUP}
        eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
        eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
        )

endfunction(add_vivado_hls_core_synth)



function(add_vivado_hls_core_impl GROUP DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH MULT ROW_EXTRA HDR_EXTRA)


string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

string(MD5 IMPL_PROJ_NAME "${DESIGN_NAME}_${RATE}_${MULT}")
string(SUBSTRING ${IMPL_PROJ_NAME} 0 12 IMPL_PROJ_NAME)


if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP})

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP})
add_vivado_hls_core_synth(${GROUP} ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} "${ROW_EXTRA}" "${HDR_EXTRA}")
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP})


add_custom_command(OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        DEPENDS gen_implement_tcl.sh
        COMMAND mkdir -p ${GROUP}/i/
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_implement_tcl.sh ${DESIGN_NAME} ${SYNTH_PROJ_NAME} ${IMPL_PROJ_NAME} ${RATE} ${MULT}  > ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        WORKING_DIRECTORY .
        )

add_custom_target(${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_implement_tcl
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
)

add_custom_command(
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_synth_utilisation.txt
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_opt_utilisation.txt
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
    #OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_checkpoint.dcp
    DEPENDS ${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_implement_tcl
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
    WORKING_DIRECTORY ${GROUP}/i/
    COMMAND mkdir -p ${IMPL_PROJ_NAME}_impl/log || true
    COMMAND mkdir -p ${IMPL_PROJ_NAME}_impl/journal || true
    COMMAND ${VIVADO_ROOT}/bin/vivado -mode batch -log ${IMPL_PROJ_NAME}_impl/log/log.txt -journal ${IMPL_PROJ_NAME}_impl/journal/journal.txt -source ${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl | tee ${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_custom_target(${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_impl_timing_txt
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
)

add_custom_target(${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation_txt
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
)

add_custom_command(OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
        DEPENDS ${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_impl_timing_txt
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
        DEPENDS ${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation_txt
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
        DEPENDS parse_impl_logs.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_impl_logs.py ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT} ${DESIGN_NAME}_${RATE}_${MULT}_${GROUP} ${DESIGN_NAME}_${RATE}_${GROUP} ${GROUP} ",${MULT}${ROW_EXTRA}"  ",iMult${HDR_EXTRA}" > ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_row_${GROUP}
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_${GROUP}
    DEPENDS ${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_implement_tcl
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS ${GROUP}_i_${DESIGN_NAME}_${RATE}_${MULT}_impl_timing_txt
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_row_${GROUP}
)

add_dependencies(eval_vhls_impl_${GROUP}
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_${GROUP}
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_row_${GROUP}
)


endfunction(add_vivado_hls_core_impl)


function(create_group GROUP)

add_custom_target(eval_vhls_synth_${GROUP})
add_custom_target(eval_vhls_impl_${GROUP})

add_custom_command(OUTPUT ${GROUP}/synth_all.csv
        DEPENDS eval_vhls_synth_${GROUP}
        DEPENDS merge_rows.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/merge_rows.py ${GROUP}/s/*.row > ${GROUP}/synth_all.csv
)
add_custom_target(build_synth_all_row_${GROUP}
        DEPENDS ${GROUP}/synth_all.csv
)

add_custom_command(OUTPUT ${GROUP}/impl_all.csv
        DEPENDS eval_vhls_impl_${GROUP}
        DEPENDS merge_rows.py
        COMMAND echo "${GROUP}/i/*.row"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/merge_rows.py ${GROUP}/i/*.row > ${GROUP}/impl_all.csv
)
add_custom_target(build_impl_all_${GROUP}
        DEPENDS ${GROUP}/impl_all.csv
)

endfunction(create_group)


#######################################################################
## FloPoCoNative


function(add_flopnat_core_synth GROUP DESIGN_NAME RATE TOPLEVEL_NAME OPERATOR_AND_ARGS ROW_EXTRA HDR_EXTRA)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

string(REPLACE "MHz" "" RAW_RATE ${RATE})

add_custom_command(OUTPUT ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        WORKING_DIRECTORY .
        COMMAND mkdir -p ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl
        COMMAND ${FLOPOCO} outputfile=${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd name=${TOPLEVEL_NAME} frequency=${RAW_RATE} target=Virtex6 ${${OPERATOR_AND_ARGS}} Wrapper

        )

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
        DEPENDS ${GROUP}/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        )

add_dependencies(eval_vhls_synth_${GROUP}
    eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
)

endfunction(add_flopnat_core_synth)


function(add_flopnat_core_impl GROUP DESIGN_NAME RATE TOPLEVEL_NAME OP_AND_ARGS2 MULT ROW_EXTRA HDR_EXTRA)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

string(MD5 IMPL_PROJ_NAME "${DESIGN_NAME}_${RATE}_${MULT}")
string(SUBSTRING ${IMPL_PROJ_NAME} 0 12 IMPL_PROJ_NAME)


if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP})

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP})
add_flopnat_core_synth( ${GROUP} ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${OP_AND_ARGS2} "${ROW_EXTRA}" "${ROW_EXTRA}" )
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP})


add_custom_command(OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        DEPENDS gen_implement_tcl.sh
        COMMAND mkdir -p ${GROUP}/i/
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_implement_tcl.sh ${DESIGN_NAME} ${SYNTH_PROJ_NAME} ${IMPL_PROJ_NAME} ${RATE} ${MULT}  > ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_synth_utilisation.txt
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_opt_utilisation.txt
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
    OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
    #OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_checkpoint.dcp
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_synth_${GROUP}
    WORKING_DIRECTORY ${GROUP}/i/
    COMMAND ls .
    COMMAND mkdir -p ${IMPL_PROJ_NAME}_impl/log || true
    COMMAND mkdir -p ${IMPL_PROJ_NAME}_impl/journal || true
    COMMAND ${VIVADO_ROOT}/bin/vivado -mode batch -log ${IMPL_PROJ_NAME}_impl/log/log.txt -journal ${IMPL_PROJ_NAME}_impl/journal/journal.txt -source ${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl | tee ${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_${GROUP}
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.tcl
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_implement.log
)

add_custom_command(OUTPUT ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_timing.txt
        DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}_impl_utilisation.txt
        DEPENDS parse_impl_logs.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_impl_logs.py ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT} ${DESIGN_NAME}_${RATE}_${MULT}_${GROUP} ${DESIGN_NAME}_${RATE}_${GROUP} ${GROUP} ${IMPL_PROJ_NAME} ",${MULT}${ROW_EXTRA}"  ",iMult${HDR_EXTRA}" > ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_row_${GROUP}
    DEPENDS ${GROUP}/i/${DESIGN_NAME}_${RATE}_${MULT}.impl.row
)

add_dependencies(eval_vhls_impl_${GROUP}
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_${GROUP}
    DEPENDS eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_row_${GROUP}
)

add_dependencies(eval_vhls_impl_${GROUP} 
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl_${GROUP}
)

endfunction(add_flopnat_core_impl)




function(add_all_flopnat_core_synth GROUP E F RATE)

set(OP_AND_ARGS FPAddDualPath wE=${E} wF=${F})
add_flopnat_core_synth(${GROUP} fp_add_flopnat_dual_${E}_${F} ${RATE} fp_add_flopnat_dual_${E}_${F} OP_AND_ARGS ,add,flopnat_dual,v1,${E},${F})

set(OP_AND_ARGS FPAddSinglePath wE=${E} wF=${F})
add_flopnat_core_synth(${GROUP} fp_add_flopnat_single_${E}_${F} ${RATE} fp_add_flopnat_single_${E}_${F} OP_AND_ARGS ,add,flopnat_single,v1,${E},${F})

set(OP_AND_ARGS FPMult wE=${E} wF=${F})
add_flopnat_core_synth(${GROUP} fp_mul_flopnat_${E}_${F} ${RATE} fp_mul_flopnat_${E}_${F} OP_AND_ARGS ,mul,flopnat,v1,${E},${F})

set(OP_AND_ARGS FPDiv wE=${E} wF=${F})
add_flopnat_core_synth(${GROUP} fp_div_flopnat_${E}_${F} ${RATE} fp_div_flopnat_${E}_${F} OP_AND_ARGS ,div,flopnat,v1,${E},${F})

endfunction(add_all_flopnat_core_synth)



function(add_all_flopnat_core_impl GROUP E F RATE MULT)

set(OP_AND_ARGS FPAddDualPath wE=${E} wF=${F})
add_vivado_hls_core_impl(${GROUP} fp_add_flopnat_dual_${E}_${F} ${RATE} fp_add_flopoco_dual_${E}_${F} OP_AND_ARGS  ${MULT}  ,add,flopnat_dual,v1,${E},${F})

set(OP_AND_ARGS FPAddSinglePath wE=${E} wF=${F})
add_flopnat_core_impl(${GROUP} fp_add_flopnat_single_${E}_${F} ${RATE} fp_add_flopoco_single_${E}_${F} OP_AND_ARGS  ${MULT}  ,add,flopnat_single,v1,${E},${F})

set(OP_AND_ARGS FPMult wE=${E} wF=${F})
add_flopnat_core_impl(${GROUP} fp_mul_flopnat_${E}_${F} ${RATE} fp_mul_flopoco_${E}_${F} OP_AND_ARGS  ${MULT}  ,mul,flopnat,v1,${E},${F})

set(OP_AND_ARGS FPDiv wE=${E} wF=${F})
add_flopnat_core_impl(${GROUP} fp_div_flopnat_${E}_${F} ${RATE} fp_div_flopoco_${E}_${F} OP_AND_ARGS  ${MULT}  ,div,flopnat,v1,${E},${F})

endfunction()


function(add_flopoco_add_single GROUP E F RATE )
add_vivado_hls_core_impl(      ${GROUP} fp_add_flopoco_single_${E}_${F} ${RATE} fp_add_flopoco_single_${E}_${F} ../fp/primitive/fp_add_flopoco_single_${E}_${F}.cpp  1.0  ",add,flopoco,flopoco_single_v1,${E},${F}" ",Primitive,Provider,Version,E,F" )
endfunction()

function(add_flopoco_mul GROUP E F RATE )
add_vivado_hls_core_impl(      ${GROUP} fp_mul_flopoco_${E}_${F} ${RATE} fp_mul_flopoco_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp  1.0  ",mul,flopoco,flopoco_v1,${E},${F}" ",Primitive,Provider,Version,E,F" )
endfunction()

function(add_flopoco_div GROUP E F RATE )
add_vivado_hls_core_impl(      ${GROUP} fp_div_flopoco_${E}_${F} ${RATE} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp  1.0  ",div,flopoco,flopoco_v1,${E},${F}" ",Primitive,Provider,Version,E,F" )
endfunction()

############################################################
## Scaling

create_group(scale_add)

add_flopoco_add_single(scale_add  4 7 200MHz) # 12
add_flopoco_add_single(scale_add  5 10 200MHz) # 16
add_flopoco_add_single(scale_add  6 13 200MHz) # 20
add_flopoco_add_single(scale_add  7 16 200MHz) # 24
add_flopoco_add_single(scale_add  8 19 200MHz) # 28
add_flopoco_add_single(scale_add  8 23 200MHz) # 32
add_flopoco_add_single(scale_add  9 26 200MHz) # 36
add_flopoco_add_single(scale_add 10 31 200MHz) # 42
add_flopoco_add_single(scale_add 10 35 200MHz) # 46
add_flopoco_add_single(scale_add 10 39 200MHz) # 50
add_flopoco_add_single(scale_add 11 42 200MHz) # 54
add_flopoco_add_single(scale_add 11 44 200MHz) # 56
add_flopoco_add_single(scale_add 11 48 200MHz) # 60
add_flopoco_add_single(scale_add 11 52 200MHz) # 64

add_vivado_hls_core_impl(scale_add fp_add_native_5_10 200MHz fp_add_native_5_10 ../fp/primitive/fp_add_native_5_10.cpp  1.0 ",add,native,native_defaultH,5,10" ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add fp_add_native_nodsp_5_10 200MHz fp_add_native_nodsp_5_10 ../fp/primitive/fp_add_native_nodsp_5_10.cpp  1.0 ",add,native,native_nodspH,5,10" ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add fp_add_native_8_23 200MHz fp_add_native_8_23 ../fp/primitive/fp_add_native_8_23.cpp  1.0 ,add,native,native_default,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add fp_add_native_nodsp_8_23 200MHz fp_add_native_nodsp_8_23 ../fp/primitive/fp_add_native_nodsp_8_23.cpp  1.0 ,add,native,native_nodsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add fp_add_native_fulldsp_8_23 200MHz fp_add_native_fulldsp_8_23 ../fp/primitive/fp_add_native_fulldsp_8_23.cpp  1.0 ,add,native,native_fulldsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add fp_add_native_11_52 200MHz fp_add_native_11_52 ../fp/primitive/fp_add_native_11_52.cpp  1.0 ,add,native,native_defaultD,11,52 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add fp_add_native_nodsp_11_52 200MHz fp_add_native_nodsp_11_52 ../fp/primitive/fp_add_native_nodsp_11_52.cpp  1.0 ,add,native,native_nodspD,11,52 ",Primitive,Provider,Version,E,F" )



create_group(scale_mul)

add_flopoco_mul(scale_mul  4 7 200MHz) # 12
add_flopoco_mul(scale_mul  5 10 200MHz) # 16
add_flopoco_mul(scale_mul  6 13 200MHz) # 20
add_flopoco_mul(scale_mul  7 16 200MHz) # 24
add_flopoco_mul(scale_mul  8 19 200MHz) # 28
add_flopoco_mul(scale_mul  8 23 200MHz) # 32
add_flopoco_mul(scale_mul  9 26 200MHz) # 36
add_flopoco_mul(scale_mul 10 31 200MHz) # 42
add_flopoco_mul(scale_mul 10 35 200MHz) # 46
add_flopoco_mul(scale_mul 10 39 200MHz) # 50
add_flopoco_mul(scale_mul 11 42 200MHz) # 54
add_flopoco_mul(scale_mul 11 44 200MHz) # 56
add_flopoco_mul(scale_mul 11 48 200MHz) # 60
add_flopoco_mul(scale_mul 11 52 200MHz) # 64

set(RATE 200MHz)
set(MULT 1.0)
add_vivado_hls_core_impl(scale_mul fp_mul_native_5_10 ${RATE} fp_mul_native_5_10 ../fp/primitive/fp_mul_native_5_10.cpp  ${MULT} ,mul,native,native_defaultH,5,10 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul fp_mul_native_8_23 ${RATE} fp_mul_native_8_23 ../fp/primitive/fp_mul_native_8_23.cpp  ${MULT} ,mul,native,native_default,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul fp_mul_native_nodsp_8_23 ${RATE} fp_mul_native_nodsp_8_23 ../fp/primitive/fp_mul_native_nodsp_8_23.cpp  ${MULT} ,mul,native,native_nodsp,8,23  ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul fp_mul_native_meddsp_8_23 ${RATE} fp_mul_native_meddsp_8_23 ../fp/primitive/fp_mul_native_meddsp_8_23.cpp  ${MULT} ,mul,native,native_meddsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul fp_mul_native_fulldsp_8_23 ${RATE} fp_mul_native_fulldsp_8_23 ../fp/primitive/fp_mul_native_fulldsp_8_23.cpp  ${MULT} ,mul,native,native_fulldsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul fp_mul_native_maxdsp_8_23 ${RATE} fp_mul_native_maxdsp_8_23 ../fp/primitive/fp_mul_native_maxdsp_8_23.cpp  ${MULT} ,mul,native,native_maxdsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul fp_mul_native_11_52 ${RATE} fp_mul_native_11_52 ../fp/primitive/fp_mul_native_11_52.cpp  ${MULT} ,mul,native,native_defaultD,11,52 ",Primitive,Provider,Version,E,F" )


set(RATE 200MHz)
set(MULT 1.0)
create_group(scale_ratio)

foreach(F1 RANGE 3 44 2)
add_vivado_hls_core_impl(scale_ratio fp_mul_flopoco_ratio_8_23_8_${F1} 200MHz fp_mul_flopoco_ratio_8_23_8_${F1} ../fp/primitive/fp_mul_flopoco_ratio_8_23_8_${F1}.cpp   1.0 ,mul,flopoco,flopoco_v1,8,23,8,${F1} ",Primitive,Provider,Version,E,F" )
endforeach(F1)

foreach(F1 RANGE 4 104 4)
add_vivado_hls_core_impl(scale_ratio fp_mul_flopoco_ratio_11_52_11_${F1} 200MHz fp_mul_flopoco_ratio_11_52_11_${F1} ../fp/primitive/fp_mul_flopoco_ratio_11_52_11_${F1}.cpp   1.0 ,mul,flopoco,flopoco_v1,11,52,11,${F1} ",Primitive,Provider,Version,E,F" )
endforeach(F1)

add_vivado_hls_core_impl(scale_ratio fp_mul_native_nodsp_8_23 ${RATE} fp_mul_native_nodsp_8_23 ../fp/primitive/fp_mul_native_nodsp_8_23.cpp  ${MULT} ,mul,native,native_nodsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_ratio fp_mul_native_meddsp_8_23 ${RATE} fp_mul_native_meddsp_8_23 ../fp/primitive/fp_mul_native_meddsp_8_23.cpp  ${MULT} ,mul,native,native_meddsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_ratio fp_mul_native_fulldsp_8_23 ${RATE} fp_mul_native_fulldsp_8_23 ../fp/primitive/fp_mul_native_fulldsp_8_23.cpp  ${MULT} ,mul,native,native_fulldsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_ratio fp_mul_native_maxdsp_8_23 ${RATE} fp_mul_native_maxdsp_8_23 ../fp/primitive/fp_mul_native_maxdsp_8_23.cpp  ${MULT} ,mul,native,native_maxdsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_ratio fp_mul_native_11_52 ${RATE} fp_mul_native_11_52 ../fp/primitive/fp_mul_native_11_52.cpp  ${MULT} ,mul,native,native_default,11,52 ",Primitive,Provider,Version,E,F" )


create_group(scale_clock)
foreach(RATE 50MHz 100MHz 150MHz 200MHz 250MHz 300MHz 350MHz 400MHz 450MHz 500MHz 550MHz 600MHz)
add_flopoco_add_single(scale_clock 8 23 ${RATE})

add_vivado_hls_core_impl(scale_clock fp_add_native_nodsp_8_23 ${RATE} fp_add_native_nodsp_8_23 ../fp/primitive/fp_add_native_nodsp_8_23.cpp  1.0 ,add,native,native_nodsp,8,23 ",Primitive,Provider,Version,E,F" )
endforeach(RATE)


function(add_benchmark_dot_par4 VARIANT RATE)
string(REPLACE "_" "," ROW_EXTRA "${VARIANT}" )
set( ROW_EXTRA ",${ROW_EXTRA}")
set( HDR_EXTRA ",ACC_T,LEFT_T,RIGHT_T,ROUND")

add_vivado_hls_core_synth(dot_product dot_product_par4_tl_${VARIANT}  ${RATE}   dot_product_par4 ../../../benchmarks/dot_product/dot_product_par4_tl_${VARIANT}.cpp       "${ROW_EXTRA}" "${HDR_EXTRA}")
add_vivado_hls_core_impl(dot_product  dot_product_par4_tl_${VARIANT}  ${RATE}   dot_product_par4 ../../../benchmarks/dot_product/dot_product_par4_tl_${VARIANT}.cpp  1.0  "${ROW_EXTRA}" "${HDR_EXTRA}")
endfunction(add_benchmark_dot_par4)

create_group(dot_product)
foreach(RATE 100MHz 200MHz)

add_benchmark_dot_par4(hls16_hls16_hls16_round ${RATE})
#add_benchmark_dot_par4(hls24_hls16_hls16_round ${RATE})
#add_benchmark_dot_par4(hls24_hls24_hls24_round ${RATE})
add_benchmark_dot_par4(hls32_hls16_hls16_round ${RATE})
add_benchmark_dot_par4(hls32_hls16_hls16_full ${RATE})
add_benchmark_dot_par4(hls32_hls32_hls16_round ${RATE})
add_benchmark_dot_par4(hls32_hls32_hls32_round ${RATE})
#add_benchmark_dot_par4(hls48_hls24_hls24_round ${RATE})
#add_benchmark_dot_par4(hls48_hls48_hls48_round ${RATE})
#add_benchmark_dot_par4(hls64_hls32_hls32_round ${RATE})
#add_benchmark_dot_par4(hls64_hls32_hls32_full ${RATE})
#add_benchmark_dot_par4(hls64_hls64_hls32_round ${RATE})
#add_benchmark_dot_par4(hls64_hls64_hls64_round ${RATE})

add_benchmark_dot_par4(nat16_nat16_nat16_round ${RATE})
add_benchmark_dot_par4(nat32_nat16_nat16_round ${RATE})
add_benchmark_dot_par4(nat32_nat32_nat16_round ${RATE})
add_benchmark_dot_par4(nat32_nat32_nat32_round ${RATE})
#add_benchmark_dot_par4(nat64_nat32_nat32_round ${RATE})
#add_benchmark_dot_par4(nat64_nat64_nat32_round ${RATE})
#add_benchmark_dot_par4(nat64_nat64_nat64_round ${RATE})

#add_benchmark_dot_par4(soft32_soft32_soft32_round ${RATE})

endforeach(RATE)



create_group(primitives)
set(RATE 200MHz)

add_vivado_hls_core_impl(primitives fp_add_native_nodsp_5_10 ${RATE} fp_add_native_5_10 ../fp/primitive/fp_add_native_5_10.cpp  1.0 ",add,native,native_defaultH,5,10" ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(primitives fp_add_native_nodsp_8_23 ${RATE} fp_add_native_nodsp_8_23 ../fp/primitive/fp_add_native_nodsp_8_23.cpp  1.0 ,add,native,native_nodsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(primitives fp_add_native_nodsp_11_52 ${RATE} fp_add_native_nodsp_11_52 ../fp/primitive/fp_add_native_nodsp_11_52.cpp  1.0 ,add,native,native_nodspD,11,52 ",Primitive,Provider,Version,E,F" )

add_vivado_hls_core_impl(primitives fp_mul_native_5_10 ${RATE} fp_mul_native_5_10 ../fp/primitive/fp_mul_native_5_10.cpp  ${MULT} ,mul,native,native_defaultH,5,10 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(primitives fp_mul_native_fulldsp_8_23 ${RATE} fp_mul_native_fulldsp_8_23 ../fp/primitive/fp_mul_native_fulldsp_8_23.cpp  ${MULT} ,mul,native,native_fulldsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(primitives fp_mul_native_11_52 ${RATE} fp_mul_native_11_52 ../fp/primitive/fp_mul_native_11_52.cpp  ${MULT} ,mul,native,native_defaultD,11,52 ",Primitive,Provider,Version,E,F" )

add_vivado_hls_core_impl(primitives fp_div_native_5_10 ${RATE} fp_div_native_5_10 ../fp/primitive/fp_div_native_5_10.cpp  1.0 ,div,native,native_default,5,10 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(primitives fp_div_native_8_23 ${RATE} fp_div_native_8_23 ../fp/primitive/fp_div_native_8_23.cpp  1.0 ,div,native,native_default,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(primitives fp_div_native_11_52 ${RATE} fp_div_native_11_52 ../fp/primitive/fp_div_native_11_52.cpp  1.0 ,div,native,native_default,11,52 ",Primitive,Provider,Version,E,F" )

add_flopoco_mul(primitives  5 10 200MHz) # 16
add_flopoco_mul(primitives  8 23 200MHz) # 32
add_flopoco_mul(primitives  11 52 200MHz) # 64

add_flopoco_add_single(primitives  5 10 200MHz) # 16
add_flopoco_add_single(primitives  8 23 200MHz) # 32
add_flopoco_add_single(primitives  11 52 200MHz) # 64

add_flopoco_div(primitives  5 10 200MHz) # 16
add_flopoco_div(primitives  8 23 200MHz) # 32
add_flopoco_div(primitives  11 52 200MHz) # 64

set(E 8)
set(F 23)
set(OP_AND_ARGS FPAddSinglePath wE=8 wF=23)
add_flopnat_core_synth(   primitives fp_add_flopnat_single_${E}_${F} ${RATE} fp_add_flopnat_single_${E}_${F} OP_AND_ARGS           ,add,flopnat_single,v1,${E},${F} ",Primitive,Provider,Version,E,F")
add_vivado_hls_core_impl( primitives fp_add_flopnat_single_${E}_${F} ${RATE} fp_add_flopnat_single_${E}_${F} OP_AND_ARGS  ${MULT}  ,add,flopnat_single,v1,${E},${F} ",Primitive,Provider,Version,E,F")

set(OP_AND_ARGS FPMult wE=8 wF=23)
add_flopnat_core_synth(   primitives fp_mul_flopnat_${E}_${F} ${RATE} fp_mul_flopnat_${E}_${F} OP_AND_ARGS           ,mul,flopnat,v1,${E},${F} ",Primitive,Provider,Version,E,F")
add_vivado_hls_core_impl( primitives fp_mul_flopnat_${E}_${F} ${RATE} fp_mul_flopnat_${E}_${F} OP_AND_ARGS  ${MULT}  ,mul,flopnat,v1,${E},${F} ",Primitive,Provider,Version,E,F")

set(OP_AND_ARGS FPDiv wE=8 wF=23)
add_flopnat_core_synth(   primitives fp_div_flopnat_${E}_${F} ${RATE} fp_div_flopnat_${E}_${F} OP_AND_ARGS           ,div,flopnat,v1,${E},${F} ",Primitive,Provider,Version,E,F")
add_vivado_hls_core_impl( primitives fp_div_flopnat_${E}_${F} ${RATE} fp_div_flopnat_${E}_${F} OP_AND_ARGS  ${MULT}  ,div,flopnat,v1,${E},${F} ",Primitive,Provider,Version,E,F")




create_group(scale_mul2)

add_flopoco_mul(scale_mul2  4 5 200MHz) # 10
add_flopoco_mul(scale_mul2  5 8 200MHz) # 14
add_flopoco_mul(scale_mul2  5 12 200MHz) # 18
add_flopoco_mul(scale_mul2  6 15 200MHz) # 22
add_flopoco_mul(scale_mul2  7 18 200MHz) # 26
add_flopoco_mul(scale_mul2  8 21 200MHz) # 30
add_flopoco_mul(scale_mul2  8 25 200MHz) # 34
add_flopoco_mul(scale_mul2  9 28 200MHz) # 38
add_flopoco_mul(scale_mul2  9 32 200MHz) # 42   
add_flopoco_mul(scale_mul2  9 36 200MHz) # 46
add_flopoco_mul(scale_mul2 10 39 200MHz) # 50
add_flopoco_mul(scale_mul2 10 43 200MHz) # 54
add_flopoco_mul(scale_mul2 11 46 200MHz) # 58
add_flopoco_mul(scale_mul2 11 50 200MHz) # 62
add_flopoco_mul(scale_mul2 11 54 200MHz) # 66
add_flopoco_mul(scale_mul2 11 58 200MHz) # 70
add_flopoco_mul(scale_mul2 12 61 200MHz) # 74

set(RATE 200MHz)
set(MULT 1.0)
add_vivado_hls_core_impl(scale_mul2 fp_mul_native_5_10 ${RATE} fp_mul_native_5_10 ../fp/primitive/fp_mul_native_5_10.cpp  ${MULT} ,mul,native,native_defaultH,5,10 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul2 fp_mul_native_fulldsp_8_23 ${RATE} fp_mul_native_fulldsp_8_23 ../fp/primitive/fp_mul_native_fulldsp_8_23.cpp  ${MULT} ,mul,native,native_fulldsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_mul2 fp_mul_native_11_52 ${RATE} fp_mul_native_11_52 ../fp/primitive/fp_mul_native_11_52.cpp  ${MULT} ,mul,native,native_defaultD,11,52 ",Primitive,Provider,Version,E,F" )



create_group(scale_add2)

add_flopoco_add_single(scale_add2  4 5 200MHz) # 10
add_flopoco_add_single(scale_add2  5 8 200MHz) # 14
add_flopoco_add_single(scale_add2  5 12 200MHz) # 18
add_flopoco_add_single(scale_add2  6 15 200MHz) # 22
add_flopoco_add_single(scale_add2  7 18 200MHz) # 26
add_flopoco_add_single(scale_add2  8 21 200MHz) # 30
add_flopoco_add_single(scale_add2  8 25 200MHz) # 34
add_flopoco_add_single(scale_add2  9 28 200MHz) # 38
add_flopoco_add_single(scale_add2  9 32 200MHz) # 42   
add_flopoco_add_single(scale_add2  9 36 200MHz) # 46
add_flopoco_add_single(scale_add2 10 39 200MHz) # 50
add_flopoco_add_single(scale_add2 10 43 200MHz) # 54
add_flopoco_add_single(scale_add2 11 46 200MHz) # 58
add_flopoco_add_single(scale_add2 11 50 200MHz) # 62
add_flopoco_add_single(scale_add2 11 54 200MHz) # 66
add_flopoco_add_single(scale_add2 11 58 200MHz) # 70
add_flopoco_add_single(scale_add2 12 61 200MHz) # 74

add_vivado_hls_core_impl(scale_add2 fp_add_native_nodsp_5_10 200MHz fp_add_native_nodsp_5_10 ../fp/primitive/fp_add_native_nodsp_5_10.cpp  1.0 ",add,native,native_nodspH,5,10" ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add2 fp_add_native_nodsp_8_23 200MHz fp_add_native_nodsp_8_23 ../fp/primitive/fp_add_native_nodsp_8_23.cpp  1.0 ,add,native,native_nodsp,8,23 ",Primitive,Provider,Version,E,F" )
add_vivado_hls_core_impl(scale_add2 fp_add_native_nodsp_11_52 200MHz fp_add_native_nodsp_11_52 ../fp/primitive/fp_add_native_nodsp_11_52.cpp  1.0 ,add,native,native_nodspD,11,52 ",Primitive,Provider,Version,E,F" )

