#!/bin/bash

# $1 = design name
# $2 = synth project name
# $3 = impl project name
# $4 = synth clock rate (e.g. 50MHz)
# $5 = synth clock rate multiplier (e.g. 1.1)

SYNTHPROJNAME="$2"
IMPLPROJNAME="$3"
CLOCKRATE="$4"
CLOCKMULT="$5"

echo "create_project -force ${IMPLPROJNAME}_impl ${IMPLPROJNAME}_impl -part xc7vx330tffg1157-3"

echo "add_files ../s/${SYNTHPROJNAME}/sim/syn/vhdl"

echo "foreach f [glob -nocomplain \"../s/${SYNTHPROJNAME}/sim/syn/vhdl/*.tcl\"] { source \$f }"

echo "update_compile_order -fileset sources_1"
echo "update_compile_order -fileset sim_1"

#echo "launch_runs synth_1"
#echo "open_run synth_1 -name synth_1"

echo "proc mhz2ns {freq} {"
echo "    return [expr 1000.0 / [ string map {MHz \"\" } \${freq} ] ]"
echo "}"

echo "set period [expr [mhz2ns ${CLOCKRATE}] / ${CLOCKMULT} ]"

echo "set constr_dir ${IMPLPROJNAME}/constr"

echo "file mkdir \${constr_dir}/constrs_1"
echo "file mkdir \${constr_dir}/constrs_1/new"
echo "close [ open \${constr_dir}/constrs_1/new/constraints.xdc w ]"
echo "add_files -fileset constrs_1 \${constr_dir}/constrs_1/new/constraints.xdc"
echo "set_property target_constrs_file \${constr_dir}/constrs_1/new/constraints.xdc [current_fileset -constrset]"

echo "synth_design -name synth_1"
echo "if {[string match \"*flopnat*\" \"${1}\"]} {"
echo "     create_clock -period \${period} -name ap_clk [get_ports clk]"
echo "} else {"
echo "     create_clock -period \${period} -name ap_clk [get_ports ap_clk]"
echo "}"
# -waveform {0.000 [expr \${period} / 2.0 ] } [get_ports ap_clk]

echo "save_constraints -force"

echo "reset_run synth_1"
echo "set_property IOB FALSE [all_outputs]"
echo "set_property IOB FALSE [all_inputs]"

echo "synth_design -name synth_1"
echo "report_utilization -file ${1}_${4}_${5}_synth_utilisation.txt -name utilization_1"

echo "opt_design"
echo "report_utilization -file ${1}_${4}_${5}_opt_utilisation.txt -name utilization_1"

echo "place_design"
echo "route_design"
echo "report_utilization -file ${1}_${4}_${5}_impl_utilisation.txt -name utilization_1"
echo "report_timing_summary -delay_type min_max -report_unconstrained -check_timing_verbose -max_paths 10 -nworst 3 -input_pins -name timing_1 -file ${1}_${4}_${5}_impl_timing.txt"

#echo "write_checkpoint -force ../../${1}_${2}_${3}_impl_checkpoint.dcp"

