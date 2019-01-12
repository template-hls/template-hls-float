#!/bin/python

import sys
import re

get_as_int = lambda m: int(m.group(1))
get_as_float = lambda m: float(m.group(1))
get_as_str = lambda m: m.group(1)

def get_value(path, match, extract = get_as_int):
    rr=re.compile(match)
    got=None
    prevLine=None
    with open(path, "r") as f:
        for l in f:
            m=rr.match(l)
            if m:
                assert got==None, prevLine
                got=extract(m)
                prevLine=l
    assert got!=None
    return got

base_path=sys.argv[1]
run_key=sys.argv[2]

proj_dir=""
if len(sys.argv)>3:
    proj_dir = sys.argv[3]
    sys.stderr.write(proj_dir+"\n")

impl_utilisation_path="{}_impl_utilisation.txt".format(base_path)
impl_timing_path="{}_impl_timing.txt".format(base_path)


topLevel = get_value(impl_utilisation_path,
    "\| Design\s+:\s+([a-zA-Z0-9_]+)",
    get_as_str
    )

part = get_value(impl_utilisation_path,
    "\| Device \s*: (.*)",
    get_as_str
    )

areaSlices = get_value(impl_utilisation_path,
    "\|\s*Slice\s*\|\s*([0-9]+)\s*\|.*")
areaSliceLs = get_value(impl_utilisation_path,
    "\|\s*SLICEL\s*\|\s*([0-9]+)\s*\|.*")
areaSliceMs = get_value(impl_utilisation_path,
    "\|\s*SLICEM\s*\|\s*([0-9]+)\s*\|.*")

areaLUTs = get_value(impl_utilisation_path,
    "\| Slice LUTs\s+\|\s+([0-9]+)\s+\|.*")
areaLUTsLogic = get_value(impl_utilisation_path,
    "\|   LUT as Logic\s+\|\s+([0-9]+)\s+\|.*")
areaLUTsMemory = get_value(impl_utilisation_path,
    "\|   LUT as Memory\s+\|\s+([0-9]+)\s+\|.*")

areaDSPs = get_value(impl_utilisation_path,
    "\| DSPs\s+\|\s+([0-9]+)\s+\|.*")
areaBRAMs = get_value(impl_utilisation_path,
    "\| Block RAM Tile\s+\|\s+([0-9]+)\s+\|.*")

(clockConstraintPeriod,clockConstraintRate)=get_value(impl_timing_path,
    "ap_clk\s+[{][0-9.]+\s+[0-9.]+[}]\s+([0-9.]+)\s+([0-9.]+).*",
    lambda m: ( float(m.group(1)), float(m.group(2)))
)

(WNS,TNS,numFailing,numEndpoints)=get_value(impl_timing_path,
    "\s*ap_clk\s+([-0-9.]+)\s+([-0-9.]+)\s+([0-9]+)\s+([0-9]+)\s+[-0-9.]+\s+[-0-9.]+\s+[0-9]+\s+[0-9]+\s+[0-9.]+\s+[0-9.]+\s+[0-9]+\s+[0-9]+\s*",
    lambda m: ( float(m.group(1)), float(m.group(2)), int(m.group(3)), int(m.group(4)) )
)

achievedClockPeriod=clockConstraintPeriod-WNS
achievedClockRate=1.0/achievedClockPeriod*1000.0

header="# {0:>16},{1:>32},{2:>40}".format("part", "topLevel", "run")
data=    "{0:>16},{1:>32},{2:>40}".format(part, topLevel, run_key)

header+=",{0:>7},{1:>7},{2:>7},{3:>7},{4:>7},{5:>7}".format("aSlice","aSliceL","aSliceM","aLUT","aLUTLog","aLUTMem") 
data  +=",{0:>7},{1:>7},{2:>7},{3:>7},{4:>7},{5:>7}".format(areaSlices,areaSliceLs,areaSliceMs,areaLUTs, areaLUTsLogic, areaLUTsMemory) 

header+=",{0:>6},{1:>6}".format("aBRAM","aDSP") 
data  +=",{0:>6},{1:>6}".format(areaBRAMs,areaDSPs) 

header+=",{0:>12},{1:>12},{2:>6},{3:>6},{4:>12},{5:>12},{6:>12}".format(
    "targetPeriod","targetRate","WNS","TNS","FailingPaths","actualPeriod","actualRate") 
data  +=",{0:>12},{1:>12},{2:>6},{3:>6},{4:>12},{5:>12},{6:>12}".format(
    clockConstraintPeriod,clockConstraintRate,WNS,TNS,numFailing,achievedClockPeriod,achievedClockRate) 



row_extra=""
if len(sys.argv)>4:
    row_extra = sys.argv[4]

print(header)
print(data+row_extra)

