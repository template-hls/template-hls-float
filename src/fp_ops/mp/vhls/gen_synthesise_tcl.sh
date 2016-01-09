#!/bin/bash

# $1 = design name
# $2 = clock rate (e.g. 50MHz)
# $3 = top-level function name
# $4 = source file path
# $5 = relative path to headers
# $6 = extra C compiler flags (if any)

# The actual project name will be ${1}_${2}

CPPFLAGS="-D__SYNTHESIS__=1 -DTHLS_SYNTHESIS=1 -DTHLS_FW_UINT_ON_AP_UINT=1 -std=c++11 -O3 -I ../../$5/../../../../include $6"

echo "puts \"${CPPFLAGS}\""

echo "open_project -reset $1_$2"
echo "open_solution -reset sim"
echo "set_part zynq"
echo "create_clock -period $2"
echo "config_schedule -verbose -effort high"
echo "config_interface -register_io scalar_all"
echo "puts [pwd]"
#echo "puts [glob ""[pwd]/../../../*""]"
echo "add_files -cflags \"${CPPFLAGS}\" ../../$4"
echo "set_top $3"
echo "csynth_design"
#echo "export_design -evaluate vhdl -format ip_catalog"
echo "close_solution"
echo "close_project"
echo "exit"
