#!/bin/python

def get_value(tree, path):
    v=tree.find(path)
    if v is None:
        raise Exception("No such attribute as '"+path)
    return v.text

def get_real(tree, path):
    v=get_value(tree, path)
    return float(v)

def get_int(tree, path):
    v=get_value(tree, path)
    return int(v)

import sys
data=sys.stdin.read()

synth_run_key=sys.argv[1]
group=sys.argv[2]
row_extra=sys.argv[3]
hdr_extra=sys.argv[4]

data='<?xml version="1.0" encoding="UTF-8"?>\n' + data;

import xml.etree.ElementTree as ET
tree = ET.fromstring(data)

part = get_value(tree, "./UserAssignments/Part")
topLevel = get_value(tree, "./UserAssignments/TopModelName")
targetPeriod = get_real(tree, "./UserAssignments/TargetClockPeriod")
targetUncertainty = get_real(tree, "./UserAssignments/ClockUncertainty")

estimatedPeriod = get_real(tree, "./PerformanceEstimates/SummaryOfTimingAnalysis/EstimatedClockPeriod")
pipelineDepth = get_int(tree, "./PerformanceEstimates/SummaryOfOverallLatency/PipelineDepth")
initiationInterval = get_int(tree, "./PerformanceEstimates/SummaryOfOverallLatency/PipelineInitiationInterval")

areaFFs = get_int(tree, "./AreaEstimates/Resources/FF")
areaLUT = get_int(tree, "./AreaEstimates/Resources/LUT")
areaBRAM = get_int(tree, "./AreaEstimates/Resources/BRAM_18K")
areaDSP = get_int(tree, "./AreaEstimates/Resources/DSP48E")

header="# {:>32},{:>16},{:>16},{:>32},{:>8},{:>8},{:>8},{:>5},{:>5}".format("sRunKey", "sGroup", "sPart", "sTopLevel", "sTargClk", "sUncerClk", "sEstClk", "sDepth", "sII")
data=  "  {:>32},{:>16},{:>16},{:>32},{:>8},{:>8},{:>8},{:>5},{:>5}".format(synth_run_key,   group,    part, topLevel, targetPeriod, targetUncertainty, estimatedPeriod, pipelineDepth, initiationInterval)

header=header+", {0:>6}, {1:>6}, {2:>6}, {3:>6}".format("sFFs","sLUTs","sBRAMS","sDSPS")
data=data    +", {0:>6}, {1:>6}, {2:>6}, {3:>6}".format(areaFFs,areaLUT,areaBRAM,areaDSP)

row_extra = sys.argv[3]

print(header+hdr_extra)
print(data+row_extra)

