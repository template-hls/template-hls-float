#!/usr/bin/env python3

import sys

hdr=None
rows=[]

for i in range(1,len(sys.argv)):
    with open(sys.argv[i],"r") as src:
        for l in src:
            l=l.strip()
            if l=="":
                continue
            if l[0]=="#":
                l=l[1:]
                l=",".join([p.strip() for p in l.split(",")])
                if hdr:
                    assert hdr==l, "Left={}, Right={}".format(hdr,l)
                else:
                    hdr=l
            else:
                l=",".join([p.strip() for p in l.split(",")])
                rows.append(l)

print(hdr)
for l in rows:
    print(l)
