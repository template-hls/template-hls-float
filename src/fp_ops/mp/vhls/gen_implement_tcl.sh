#!/bin/bash

# $1 = design name
# $2 = synth clock rate (e.g. 50MHz)
# $3 = synth clock rate multiplier (e.g. 1.1)

# The actual project name will be ${1}_${2}_${3}

echo "create_project -force ${1}_${2}_${3}_impl ${1}_${2}_${3}_impl -part xc7z020clg484-1"

echo "add_files ../s/${1}_${2}/sim/syn/vhdl"

echo "foreach f [glob -nocomplain \"../s/${1}_${2}/sim/syn/vhdl/*.tcl\"] { source \$f }"

echo "update_compile_order -fileset sources_1"
echo "update_compile_order -fileset sim_1"

#echo "launch_runs synth_1"
#echo "open_run synth_1 -name synth_1"

echo "proc mhz2ns {freq} {"
echo "    return [expr 1000.0 / [ string map {MHz \"\" } \${freq} ] ]"
echo "}"

echo "set period [expr [mhz2ns $2] / $3 ]"

echo "synth_design -name synth_1"
echo "create_clock -period \${period} -name ap_clk [get_ports ap_clk]"
# -waveform {0.000 [expr \${period} / 2.0 ] } [get_ports ap_clk]

echo "set constr_dir $1_$2_$3/constr"

echo "file mkdir $constr_dir/constrs_1"
echo "file mkdir $constr_dir/constrs_1/new"
echo "close [ open $constr_dir/constrs_1/new/constraints.xdc w ]"
echo "add_files -fileset constrs_1 $constr_dir/constrs_1/new/constraints.xdc"
echo "set_property target_constrs_file $constr_dir/constrs_1/new/constraints.xdc [current_fileset -constrset]"
echo "save_constraints -force"

echo "reset_run synth_1"
echo "set_property IOB FALSE [all_outputs]"
echo "set_property IOB FALSE [all_inputs]"

echo "synth_design -name synth_1"
echo "report_utilization -file ../../${1}_${2}_${3}_synth_utilisation.txt -name utilization_1"

echo "opt_design"
echo "report_utilization -file ../../${1}_${2}_${3}_opt_utilisation.txt -name utilization_1"

echo "place_design"
echo "route_design"
echo "report_utilization -file ../../${1}_${2}_${3}_impl_utilisation.txt -name utilization_1"
echo "report_timing_summary -delay_type min_max -report_unconstrained -check_timing_verbose -max_paths 10 -nworst 3 -input_pins -name timing_1 -file ../../${1}_${2}_${3}_impl_timing.txt"

echo "write_checkpoint -force ../../${1}_${2}_${3}_impl_checkpoint.dcp"
        
