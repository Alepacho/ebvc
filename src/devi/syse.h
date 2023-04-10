#ifndef SYSE_H
#define SYSE_H

#include "../ebvc.h"

// System Eight Specs:
/*
Mode:
IN | 0000 : System Power Off
IN | 0001 :
IN | 0010 :
IN | 0011 :
IN | 01rr : Write to console
IN | 10rr : 
IN | 11rr : 

OUT | 00rr : 
OUT | 01rr : Read from console
OUT | 10rr : 
OUT | 11rr : 

*/
#define SYSE_IN_BREAK 0b0000
// #define SYSE_IN_WRITE 0b01

// #define SYSE_OUT_READ 0b0011

ubyte syse_input(ebvc_t* ebvc, ubyte mode);
void syse_output(ebvc_t* ebvc, ubyte mode);

#endif // SYSE_H