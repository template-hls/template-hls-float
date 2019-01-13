# The build/synth/ -> b/i/ change is because of path length limitations


#file(RELATIVE_PATH THLS_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})
string(REPLACE "/cygdrive/e" "e:/" THLS_SOURCE_DIR "${CMAKE_SOURCE_DIR}" )

find_program(FLOPOCO flopoco HINTS /cygdrive/e/external/flopoco/trunk/gcc ${CMAKE_SOURCE_DIR}/bin)

add_custom_target(eval_vhls_synth)
add_custom_target(eval_flopnat_synth)
add_custom_target(eval_vhls_impl)
add_custom_target(eval_flopnat_impl)


function(add_vivado_hls_core_synth DESIGN_NAME RATE TOPLEVEL_NAME  DESIGN_SRC_PATH ROW_EXTRA)

if(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
        return()
endif(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS gen_synthesise_tcl.sh
        COMMAND mkdir -p b/s
        COMMAND echo $$(pwd)
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gen_synthesise_tcl.sh ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${SYNTH_PROJ_NAME} ${DESIGN_SRC_PATH} ${THLS_SOURCE_DIR} "" > b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        WORKING_DIRECTORY .
        )

add_custom_command(OUTPUT b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS all_generated_primitive_cpp
        DEPENDS b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DESIGN_SRC_PATH}
        WORKING_DIRECTORY b/s
        COMMAND ${VIVADO_HLS}/bin/vivado_hls -f ${DESIGN_NAME}_${RATE}_synthesise.tcl -l ${DESIGN_NAME}_${RATE}.log
        COMMAND rm -rf ${DESIGN_NAME}_${RATE}/sim/.autopilot
        )

add_custom_command(OUTPUT b/s/${DESIGN_NAME}_${RATE}.syn.row
        DEPENDS b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS parse_rpt.py
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parse_rpt.py ${ROW_EXTRA}  < b/s/${SYNTH_PROJ_NAME}/sim/syn/report/${DESIGN_NAME}_csynth.xml > b/s/${DESIGN_NAME}_${RATE}.syn.row
)

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth
        DEPENDS b/s/${DESIGN_NAME}_${RATE}_synthesise.tcl
        DEPENDS b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        DEPENDS b/s/${DESIGN_NAME}_${RATE}.syn.row
        )

add_dependencies(eval_vhls_synth eval_vhls_${DESIGN_NAME}_${RATE}_synth)

endfunction(add_vivado_hls_core_synth)



function(add_vivado_hls_core_impl DESIGN_NAME RATE TOPLEVEL_NAME DESIGN_SRC_PATH MULT ROW_EXTRA)

if(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl)
        return()
endif(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
add_vivado_hls_core_synth( ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${DESIGN_SRC_PATH} ${ROW_EXTRA})
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

string(MD5 IMPL_PROJ_NAME "${DESIGN_NAME}_${RATE}_${MULT}")
string(SUBSTRING ${IMPL_PROJ_NAME} 0 12 IMPL_PROJ_NAME)


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

add_dependencies(eval_vhls_impl
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
)




endfunction(add_vivado_hls_core_impl)


add_custom_command(OUTPUT b/synth_all.row
        DEPENDS eval_vhls_synth
        COMMAND cat b/s/*.row > b/synth_all.row
)
add_custom_target(build_synth_all_row
        DEPENDS b/synth_all.row
)

set(MULTIPLIERS 1.0 1.1 1.2 1.3 1.4 1.5)

########################################################################################
## Native

### Synth

function(add_all_native_half_core_synth  RATE)
add_vivado_hls_core_synth(fp_add_native_5_10 ${RATE} fp_add_native_5_10 ../fp/primitive/fp_add_native_5_10.cpp   ,add,native,default,5,10)
add_vivado_hls_core_synth(fp_mul_native_5_10 ${RATE} fp_mul_native_5_10 ../fp/primitive/fp_mul_native_5_10.cpp   ,mul,native,default,5,10)
add_vivado_hls_core_synth(fp_div_native_5_10 ${RATE} fp_div_native_5_10 ../fp/primitive/fp_div_native_5_10.cpp   ,div,native,default,5,10)
endfunction(add_all_native_half_core_synth)


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

function(add_all_native_half_core_impl  RATE MULT)
add_vivado_hls_core_impl(fp_add_native_5_10 ${RATE} fp_add_native_5_10 ../fp/primitive/fp_add_native_5_10.cpp  ${MULT} ,add,native,default,5,10)
add_vivado_hls_core_impl(fp_mul_native_5_10 ${RATE} fp_mul_native_5_10 ../fp/primitive/fp_mul_native_5_10.cpp  ${MULT} ,mul,native,default,5,10)
add_vivado_hls_core_impl(fp_div_native_5_10 ${RATE} fp_div_native_5_10 ../fp/primitive/fp_div_native_5_10.cpp  ${MULT} ,div,native,default,5,10)
endfunction(add_all_native_half_core_impl)

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
#add_vivado_hls_core_impl(fp_add_flopoco_single_v3_${E}_${F} ${RATE} fp_add_flopoco_single_v3_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v3_${E}_${F}.cpp  ${MULT}  ,add,flopoco_single,v3,${E},${F})
#add_vivado_hls_core_impl(fp_add_flopoco_single_v4_${E}_${F} ${RATE} fp_add_flopoco_single_v4_${E}_${F} ../fp/primitive/fp_add_flopoco_single_v4_${E}_${F}.cpp  ${MULT}  ,add,flopoco_single,v4,${E},${F})

add_vivado_hls_core_impl(fp_mul_flopoco_${E}_${F} ${RATE} fp_mul_flopoco_${E}_${F} ../fp/primitive/fp_mul_flopoco_${E}_${F}.cpp ${MULT} ,mul,flopoco,v1,${E},${F})

add_vivado_hls_core_impl(fp_div_flopoco_${E}_${F} ${RATE} fp_div_flopoco_${E}_${F} ../fp/primitive/fp_div_flopoco_${E}_${F}.cpp  ${MULT} ,div,flopoco,v1,${E},${F})
add_vivado_hls_core_impl(fp_div_flopoco_v2_${E}_${F} ${RATE} fp_div_flopoco_v2_${E}_${F} ../fp/primitive/fp_div_flopoco_v2_${E}_${F}.cpp ${MULT} ,div,flopoco,v2,${E},${F})
#add_vivado_hls_core_impl(fp_div_flopoco_v3_${E}_${F} ${RATE} fp_div_flopoco_v3_${E}_${F} ../fp/primitive/fp_div_flopoco_v3_${E}_${F}.cpp ${MULT} ,div,flopoco,v3,${E},${F})
endfunction(add_all_flopoco_core_impl)


#######################################################################
## FloPoCoNative


function(add_flopnat_core_synth DESIGN_NAME RATE TOPLEVEL_NAME OPERATOR_AND_ARGS ROW_EXTRA)

if(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
        return()
endif(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

string(REPLACE "MHz" "" RAW_RATE ${RATE})

add_custom_command(OUTPUT b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        WORKING_DIRECTORY .
        COMMAND mkdir -p b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl
        COMMAND ${FLOPOCO} outputfile=b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd name=${TOPLEVEL_NAME} frequency=${RAW_RATE} target=Virtex6 ${${OPERATOR_AND_ARGS}} Wrapper

        )

add_custom_target(eval_vhls_${DESIGN_NAME}_${RATE}_synth
        DEPENDS b/s/${SYNTH_PROJ_NAME}/sim/syn/vhdl/${DESIGN_NAME}.vhd
        )

add_dependencies(eval_flopnat_synth
    eval_vhls_${DESIGN_NAME}_${RATE}_synth
)

endfunction(add_flopnat_core_synth)


function(add_flopnat_core_impl DESIGN_NAME RATE TOPLEVEL_NAME OP_AND_ARGS2 MULT ROW_EXTRA)

string(MD5 SYNTH_PROJ_NAME "${DESIGN_NAME}_${RATE}")
string(SUBSTRING ${SYNTH_PROJ_NAME} 0 12 SYNTH_PROJ_NAME)

if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl )
        return()
endif(TARGET eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl)


if( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

else( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)
add_flopnat_core_synth( ${DESIGN_NAME} ${RATE} ${TOPLEVEL_NAME} ${OP_AND_ARGS2} ${ROW_EXTRA})
endif( TARGET eval_vhls_${DESIGN_NAME}_${RATE}_synth)

string(MD5 IMPL_PROJ_NAME "${DESIGN_NAME}_${RATE}_${MULT}")
string(SUBSTRING ${IMPL_PROJ_NAME} 0 12 IMPL_PROJ_NAME)


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
    COMMAND ls .
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

add_dependencies(eval_vhls_impl
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
)

add_dependencies(eval_flopnat_impl
    eval_vhls_${DESIGN_NAME}_${RATE}_${MULT}_impl
)

endfunction(add_flopnat_core_impl)




function(add_all_flopnat_core_synth E F RATE)

set(OP_AND_ARGS FPAddDualPath wE=${E} wF=${F})
add_flopnat_core_synth(fp_add_flopnat_dual_${E}_${F} ${RATE} fp_add_flopnat_dual_${E}_${F} OP_AND_ARGS ,add,flopnat_dual,v1,${E},${F})

set(OP_AND_ARGS FPAddSinglePath wE=${E} wF=${F})
add_flopnat_core_synth(fp_add_flopnat_single_${E}_${F} ${RATE} fp_add_flopnat_single_${E}_${F} OP_AND_ARGS ,add,flopnat_single,v1,${E},${F})

set(OP_AND_ARGS FPMult wE=${E} wF=${F})
add_flopnat_core_synth(fp_mul_flopnat_${E}_${F} ${RATE} fp_mul_flopnat_${E}_${F} OP_AND_ARGS ,mul,flopnat,v1,${E},${F})

set(OP_AND_ARGS FPDiv wE=${E} wF=${F})
add_flopnat_core_synth(fp_div_flopnat_${E}_${F} ${RATE} fp_div_flopnat_${E}_${F} OP_AND_ARGS ,div,flopnat,v1,${E},${F})

endfunction(add_all_flopnat_core_synth)



function(add_all_flopnat_core_impl E F RATE MULT)

set(OP_AND_ARGS FPAddDualPath wE=${E} wF=${F})
add_flopnat_core_impl(fp_add_flopnat_dual_${E}_${F} ${RATE} fp_add_flopoco_dual_${E}_${F} OP_AND_ARGS  ${MULT}  ,add,flopnat_dual,v1,${E},${F})

set(OP_AND_ARGS FPAddSinglePath wE=${E} wF=${F})
add_flopnat_core_impl(fp_add_flopnat_single_${E}_${F} ${RATE} fp_add_flopoco_single_${E}_${F} OP_AND_ARGS  ${MULT}  ,add,flopnat_single,v1,${E},${F})

set(OP_AND_ARGS FPMult wE=${E} wF=${F})
add_flopnat_core_impl(fp_mul_flopnat_${E}_${F} ${RATE} fp_mul_flopoco_${E}_${F} OP_AND_ARGS  ${MULT}  ,mul,flopnat,v1,${E},${F})

set(OP_AND_ARGS FPDiv wE=${E} wF=${F})
add_flopnat_core_impl(fp_div_flopnat_${E}_${F} ${RATE} fp_div_flopoco_${E}_${F} OP_AND_ARGS  ${MULT}  ,div,flopnat,v1,${E},${F})

endfunction()



foreach(M 100MHz 200MHz 300MHz )
        add_all_native_half_core_impl(${M} 1.0)
        add_all_native_single_core_impl(${M} 1.0)
        add_all_native_double_core_impl(${M} 1.0)

        add_all_flopoco_core_impl(5  6 ${M} 1.0)
        add_all_flopoco_core_impl(5  7 ${M} 1.0)
        add_all_flopoco_core_impl(5  8 ${M} 1.0)
        add_all_flopoco_core_impl(5  9 ${M} 1.0)
        add_all_flopoco_core_impl(5 10 ${M} 1.0) # Standard 16 bit
        add_all_flopoco_core_impl(5 11 ${M} 1.0)
        add_all_flopoco_core_impl(5 12 ${M} 1.0)
        add_all_flopoco_core_impl(5 13 ${M} 1.0)
        add_all_flopoco_core_impl(5 14 ${M} 1.0)
        add_all_flopoco_core_impl(5 15 ${M} 1.0)
        add_all_flopoco_core_impl(5 16 ${M} 1.0)
        add_all_flopoco_core_impl(5 17 ${M} 1.0)
        add_all_flopoco_core_impl(5 18 ${M} 1.0)

        add_all_flopoco_core_impl(6 13 ${M} 1.0)  # Standard 20 bit

        add_all_flopoco_core_impl(7 16 ${M} 1.0)  # Standard 24-bit

        add_all_flopoco_core_impl(8 7 ${M} 1.0)   # bfloat16 (NVidia)
        add_all_flopoco_core_impl(8 9 ${M} 1.0)
        add_all_flopoco_core_impl(8 11 ${M} 1.0)
        add_all_flopoco_core_impl(8 13 ${M} 1.0)
        add_all_flopoco_core_impl(8 15 ${M} 1.0)
        add_all_flopoco_core_impl(8 17 ${M} 1.0)
        add_all_flopoco_core_impl(8 19 ${M} 1.0)
        add_all_flopoco_core_impl(8 21 ${M} 1.0)
        add_all_flopoco_core_impl(8 23 ${M} 1.0)  # Standard 32-bit
        add_all_flopoco_core_impl(8 25 ${M} 1.0)
        add_all_flopoco_core_impl(8 27 ${M} 1.0)
        add_all_flopoco_core_impl(8 29 ${M} 1.0)
        add_all_flopoco_core_impl(8 31 ${M} 1.0)

        add_all_flopoco_core_impl(9 30 ${M} 1.0)  # Standard 40-bit

        add_all_flopoco_core_impl(9 38 ${M} 1.0)  # Standard 48-bit

        add_all_flopoco_core_impl(10 45 ${M} 1.0)  # Standard 56-bit

        add_all_flopoco_core_impl(11 52 ${M} 1.0)  # Standard 64-bit

        add_all_flopnat_core_impl(5 10 ${M} 1.0)
        add_all_flopnat_core_impl(6 13 ${M} 1.0)
        add_all_flopnat_core_impl(7 16 ${M} 1.0)
        add_all_flopnat_core_impl(8 23 ${M} 1.0)
        add_all_flopnat_core_impl(9 30 ${M} 1.0)
        add_all_flopnat_core_impl(9 38 ${M} 1.0)
        add_all_flopnat_core_impl(10 45 ${M} 1.0)
        add_all_flopnat_core_impl(11 52 ${M} 1.0)


endforeach(M)

foreach(M 20MHz 40MHz 60MHz 80MHz 100MHz 120MHz 140MHz 160MHz 180MHz 200MHz 220MHz 240MHz 260MHz 280MHz 300MHz 320MHz 340MHz 360MHz 380MHz 400MHz  )
        add_all_flopoco_core_impl(8 23 ${M} 1.0)
        add_all_native_single_core_impl(${M} 1.0)

        add_all_flopoco_core_impl(11 52 ${M} 1.0)
        add_all_native_double_core_impl(${M} 1.0)
endforeach(M)


add_custom_command(OUTPUT b/impl_all.row
        DEPENDS eval_vhls_impl
        COMMAND cat b/i/*.impl.row > b/impl_all.row
)
add_custom_target(build_impl_all_row
        DEPENDS b/impl_all.row
)

