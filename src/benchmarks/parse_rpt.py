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

header="# {0:>16},{1:>32},{2:>8},{3:>8},{4:>8},{5:>5},{6:>5}".format("part", "topLevel", "targClk", "uncerClk", "estClk", "depth", "II")
data=  "  {0:>16},{1:>32},{2:>8},{3:>8},{4:>8},{5:>5},{6:>5}".format(part, topLevel, targetPeriod, targetUncertainty, estimatedPeriod, pipelineDepth, initiationInterval)

header=header+", {0:>6}, {1:>6}, {2:>6}, {3:>6}".format("FFs","LUTs","BRAMS","DSPS")
data=data    +", {0:>6}, {1:>6}, {2:>6}, {3:>6}".format(areaFFs,areaLUT,areaBRAM,areaDSP)

row_extra = sys.argv[1]

print(header)
print(data+row_extra)

