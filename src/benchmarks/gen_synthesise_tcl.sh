#!/bin/bash

# $1 = design name
# $2 = clock rate (e.g. 50MHz)
# $3 = top-level function name
# $4 = Project name
# $5 = source file path
# $6 = path to repo root
# $7 = extra C compiler flags (if any)

DESIGNNAME="$1"
CLOCKRATE="$2"
TOPLEVEL="$3"

PROJNAME="$4"

SRCFILEPATH="$5"
REPOPATH="$6"
EXTRACOMPILERFLAGS="$7"

CPPFLAGS="-D__SYNTHESIS__=1 -DTHLS_SYNTHESIS=1 -DTHLS_FW_UINT_ON_AP_UINT=1 -std=c++11 -O3 -I ${REPOPATH}/include ${EXTRACOMPILERFLAGS}"

echo "puts \"${CPPFLAGS}\""

echo "open_project -reset ${PROJNAME}"
echo "open_solution -reset sim"
echo "set_part {xc7vx330tffg1157-3}"
echo "create_clock -period ${CLOCKRATE}"
echo "config_schedule -verbose -effort high"
echo "config_interface -register_io scalar_all"
echo "puts [pwd]"
#echo "puts [glob ""[pwd]/../../../*""]"
echo "add_files -cflags \"${CPPFLAGS}\" ${SRCFILEPATH}"
echo "set_top ${TOPLEVEL}"
echo "csynth_design"
#echo "export_design -evaluate vhdl -format ip_catalog"
echo "close_solution"
echo "close_project"
echo "exit"
