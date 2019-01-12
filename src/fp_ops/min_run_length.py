import z3

for WF in range(4,32):
    rs=[0,0]
    for c in range(2):
        for r in range(1,2*WF+2):
            x=z3.BitVec('x',WF)
            y=z3.BitVec('y',WF)
            rv=z3.BitVec('rv',r)

            s=z3.Solver()
            s.add( x <= y )

            zero=z3.BitVecVal(0, 1)
            zeros=z3.BitVecVal(0, 1+WF)
            one=z3.BitVecVal(1, 1)
            xf=z3.Concat([zeros, one, x])
            yf=z3.Concat([zeros, one, y])

            p=z3.BitVec('p',2*WF+2)
            s.add(p == xf*yf)

            print("r={}, c={}".format(r,c))

            if c==0:
                # Only deal with lower cases, not overflow
                lower_cond = z3.Extract(2*WF+1,2*WF+1,p) == zero
                lower_gap=z3.Extract(2*WF,2*WF+ 1-r, p)
                s.add(lower_gap==rv)
                s.add(lower_cond)
                s.add(lower_gap==z3.BitVecVal(2**r-1, r))
            else:
                # deal with upper cases, i.e. overflow
                upper_cond = z3.Extract(2*WF+1,2*WF+1,p) == one
                upper_gap  = z3.Extract(2*WF+1,2*WF+1-(r-1), p)
                s.add(upper_gap==rv)
                s.add(upper_cond)
                s.add( upper_gap==z3.BitVecVal(2**r-1, r) )

            if z3.unsat == s.check():
                break
            print(s.model())
        
        rs[c]=r-1

    print("WF={}, rs={}".format(WF,rs))