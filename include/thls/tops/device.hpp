

struct TargetProperties
{
    
    static const int lutInputs = 6;
    
    static const int bramDepth = 1024;
    static const int bramWidth = 36;
    
    /*!
        Suggested adder segment size for decent clock rate.
        This should be about right with both a carry in and
        a carry out. The adder segment refers to the longer
        of the two inputs.
    */
    static const int adderSegmentLength = 32;
  
    /*! 
    static const int dspMajorLength = 24;
    static const int dspMinorLength = 18;
};
