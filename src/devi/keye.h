#ifndef KEYE_H
#define KEYE_H
// https://github.com/lucaspiller/dcpu-specifications/blob/master/keyboard.txt

#include "../ebvc.h"

#define KEYE_IN_PRESS_RA    0b0000 // Возвращает в RD флаг если клавиша из регистра RA была нажата
#define KEYE_IN_PRESS_RB    0b0001 // Возвращает в RD флаг если клавиша из регистра RB была нажата
#define KEYE_IN_PRESS_RC    0b0010 // Возвращает в RD флаг если клавиша из регистра RC была нажата
#define KEYE_IN_FLAGS       0b0011 // Получить статус в регистр RD
#define KEYE_IN_UNDEF_4     0b0100 //
#define KEYE_IN_UNDEF_5     0b0101 //
#define KEYE_IN_UNDEF_6     0b0110 //
#define KEYE_IN_UNDEF_7     0b0111 //
#define KEYE_IN_UNDEF_8     0b1000 //
#define KEYE_IN_UNDEF_9     0b1001 //
#define KEYE_IN_UNDEF_A     0b1010 //
#define KEYE_IN_UNDEF_B     0b1011 //
#define KEYE_IN_UNDEF_C     0b1100 //
#define KEYE_IN_UNDEF_D     0b1101 //
#define KEYE_IN_UNDEF_E     0b1110 //
#define KEYE_IN_UNDEF_F     0b1111 //

#define KEYE_OUT_CLEAR      0b0000 // Очистка буфера клавиатуры
#define KEYE_OUT_UNDEF_1    0b0001 // 
#define KEYE_OUT_UNDEF_2    0b0010 // 
#define KEYE_OUT_UNDEF_3    0b0011 // 
#define KEYE_OUT_UNDEF_4    0b0100 // 
#define KEYE_OUT_UNDEF_5    0b0101 // 
#define KEYE_OUT_UNDEF_6    0b0110 // 
#define KEYE_OUT_UNDEF_7    0b0111 // 
#define KEYE_OUT_UNDEF_8    0b1000 // 
#define KEYE_OUT_UNDEF_9    0b1001 // 
#define KEYE_OUT_UNDEF_A    0b1010 // 
#define KEYE_OUT_UNDEF_B    0b1011 // 
#define KEYE_OUT_UNDEF_C    0b1100 // 
#define KEYE_OUT_UNDEF_D    0b1101 // 
#define KEYE_OUT_UNDEF_E    0b1110 // 
#define KEYE_OUT_UNDEF_F    0b1111 // 

#define KEYE_KEYS_COUNT     256

typedef struct keye_t {
    bool keys[KEYE_KEYS_COUNT];
    // bool interrupts; // 
    // uword ip_addr : 12; // interrupt address pointer;
} keye_t;

void keye_init(keye_t* keye);
void keye_update(keye_t* keye, ubyte *keys);
void keye_term(keye_t* keye);


ubyte keye_input(keye_t* keye, ebvc_t* ebvc, ubyte mode);
void keye_output(keye_t* keye, ebvc_t* ebvc, ubyte mode);

#endif // KEYE_H
