#ifndef SYSE_H
#define SYSE_H

#include "../ebvc.h"

// System Eight Specs:
// TODO: add specs

// #define SYSE_IN_UNDEF_0     0b0000 //
// #define SYSE_IN_UNDEF_1     0b0001 //
// #define SYSE_IN_UNDEF_2     0b0010 //
// #define SYSE_IN_UNDEF_3     0b0011 //
// #define SYSE_IN_UNDEF_4     0b0100 //
// #define SYSE_IN_UNDEF_5     0b0101 //
// #define SYSE_IN_UNDEF_6     0b0110 //
// #define SYSE_IN_UNDEF_7     0b0111 //
// #define SYSE_IN_UNDEF_8     0b1000 //
// #define SYSE_IN_UNDEF_9     0b1001 //
// #define SYSE_IN_UNDEF_A     0b1010 //
// #define SYSE_IN_UNDEF_B     0b1011 //
// #define SYSE_IN_RAND_RA     0b1100 // Заносит случайное значение в регистр RA
// #define SYSE_IN_RAND_RB     0b1101 // Заносит случайное значение в регистр RB
// #define SYSE_IN_RAND_RC     0b1110 // Заносит случайное значение в регистр RC
// #define SYSE_IN_UNDEF_F     0b1111 //


#define SYSE_OUT_BREAK      0b0000
#define SYSE_OUT_WRITE      0b0001
// #define SYSE_OUT_UNDEF_2    0b0010 // 
// #define SYSE_OUT_UNDEF_3    0b0011 // 
// #define SYSE_OUT_UNDEF_4    0b0100 // 
// #define SYSE_OUT_UNDEF_5    0b0101 // 
// #define SYSE_OUT_UNDEF_6    0b0110 // 
// #define SYSE_OUT_UNDEF_7    0b0111 // 
// #define SYSE_OUT_UNDEF_8    0b1000 // 
// #define SYSE_OUT_UNDEF_9    0b1001 // 
// #define SYSE_OUT_UNDEF_A    0b1010 // 
// #define SYSE_OUT_UNDEF_B    0b1011 // 
// #define SYSE_OUT_UNDEF_C    0b1100 // 
// #define SYSE_OUT_UNDEF_D    0b1101 // 
// #define SYSE_OUT_UNDEF_E    0b1110 // 
// #define SYSE_OUT_UNDEF_F    0b1111 // 

ubyte syse_input(ebvc_t* ebvc, ubyte mode);
void syse_output(ebvc_t* ebvc, ubyte mode);

#endif // SYSE_H
