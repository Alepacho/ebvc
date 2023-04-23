#ifndef TEMP_H
#define TEMP_H

#include "../ebvc.h"

#define TEMP_IN_UNDEF_0     0b0000 //
#define TEMP_IN_UNDEF_1     0b0001 //
#define TEMP_IN_UNDEF_2     0b0010 //
#define TEMP_IN_UNDEF_3     0b0011 //
#define TEMP_IN_UNDEF_4     0b0100 //
#define TEMP_IN_UNDEF_5     0b0101 //
#define TEMP_IN_UNDEF_6     0b0110 //
#define TEMP_IN_UNDEF_7     0b0111 //
#define TEMP_IN_UNDEF_8     0b1000 //
#define TEMP_IN_UNDEF_9     0b1001 //
#define TEMP_IN_UNDEF_A     0b1010 //
#define TEMP_IN_UNDEF_B     0b1011 //
#define TEMP_IN_UNDEF_C     0b1100 //
#define TEMP_IN_UNDEF_D     0b1101 //
#define TEMP_IN_UNDEF_E     0b1110 //
#define TEMP_IN_UNDEF_F     0b1111 //

#define TEMP_OUT_UNDEF_0    0b0000 // 
#define TEMP_OUT_UNDEF_1    0b0001 // 
#define TEMP_OUT_UNDEF_2    0b0010 // 
#define TEMP_OUT_UNDEF_3    0b0011 // 
#define TEMP_OUT_UNDEF_4    0b0100 // 
#define TEMP_OUT_UNDEF_5    0b0101 // 
#define TEMP_OUT_UNDEF_6    0b0110 // 
#define TEMP_OUT_UNDEF_7    0b0111 // 
#define TEMP_OUT_UNDEF_8    0b1000 // 
#define TEMP_OUT_UNDEF_9    0b1001 // 
#define TEMP_OUT_UNDEF_A    0b1010 // 
#define TEMP_OUT_UNDEF_B    0b1011 // 
#define TEMP_OUT_UNDEF_C    0b1100 // 
#define TEMP_OUT_UNDEF_D    0b1101 // 
#define TEMP_OUT_UNDEF_E    0b1110 // 
#define TEMP_OUT_UNDEF_F    0b1111 // 

ubyte temp_input(ebvc_t* ebvc, ubyte mode);
void temp_output(ebvc_t* ebvc, ubyte mode);

#endif // TEMP_H