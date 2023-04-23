#ifndef DISE_H
#define DISE_H

#include "../ebvc.h"

// DISPLAY EIGHT SPECS:
// 2 modes:
//   Graphics:  64x32
//   Text:      16x8x2 (4x4 fonts)

// = ATTRS ===
// ic cc cc cc 
// ===========

// https://ru.wikipedia.org/wiki/%D0%A3%D0%BF%D1%80%D0%B0%D0%B2%D0%BB%D1%8F%D1%8E%D1%89%D0%B8%D0%B5_%D1%81%D0%B8%D0%BC%D0%B2%D0%BE%D0%BB%D1%8B
// ASCII:
/*
 !"#$%&'()*+,-./
0123456789:;<=>?
@ABCDEFGHIJKLMNO
PQRSTUVWXYZ[\]^_
`abcdefghijklmno
pqrstuvwxyz{|}~
*/
static const uword dise_font[] = {
    0x0000, // 0x00 NULL
    0x0000, // 0x01 ^A : Unused 
    0x0000, // 0x02 ^B : Unused 
    0x0000, // 0x03 ^C : End of Text (Interrupt)
    0x0000, // 0x04 ^D : End of Transmission
    0x0000, // 0x05 ^E : Unused 
    0x0000, // 0x06 ^F : Unused
    0x0000, // 0x07 ^G : Bell (\a)
    0x0000, // 0x08 ^H : Backspace (\b)
    0x0000, // 0x09 ^I : Horizontal Tab (\t)
    0x0000, // 0x0A ^J : Line Feed (\n)
    0x0000, // 0x0B ^K : Unused
    0x0000, // 0x0C ^L : Form Feed (Clear Screen)
    0x0000, // 0x0D ^M : Carriage Return (\r)
    0x0000, // 0x0E ^N : Unused
    0x0000, // 0x0F ^O : Unused
    0x0000, // 0x10 ^P : Unused
    0x0000, // 0x11 ^Q : Unused
    0x0000, // 0x12 ^R : Unused
    0x0000, // 0x13 ^S : Unused
    0x0000, // 0x14 ^T : Unused
    0x0000, // 0x15 ^U : Unused
    0x0000, // 0x16 ^V : Unused
    0x0000, // 0x17 ^W : Unused
    0x0000, // 0x18 ^X : Unused
    0x0000, // 0x19 ^Y : Unused
    0x0000, // 0x1A ^Z : Unused
    0x0000, // 0x1B ^[ : Escape
    0x0000, // 0x1C ^\ : Unused
    0x0000, // 0x1D ^] : Unused
    0x0000, // 0x1E ^^ : Unused
    0x0000, // 0x1F ^? : Unused

    0x0000, // [20/A0] space
    0x4400, // [21/A1] ! 010 010 000 000
    0xAA00, // [22/A2] " 101 101 000 000
    0xAEE0, // [23/A3] # 101 111 111 000
    0x64C4, // [24/A4] $ 011 010 110 010
    0xCE60, // [25/A5] % 110 111 011 000
    0x4C60, // [26/A6] & 010 110 011 000
    0x4000, // [27/A7] ' 010 000 000 000
    0x4840, // [28/A8] ( 010 100 010 000
    0x4240, // [29/A9] ) 010 001 010 000
    0x6600, // [2A/AA] * 011 011 000 000
    0x4E40, // [2B/AB] + 010 111 010 000
    0x0088, // [2C/AC] , 000 000 100 100
    0x0E00, // [2D/AD] - 000 111 000 000
    0x0080, // [2E/AE] . 000 000 100 000
    0x2480, // [2F/AF] / 001 010 100 000

    0x4A40, // [30/B0] 0 010 101 010 000
    0xC4E0, // [31/B1] 1 110 010 111 000
    0xC460, // [32/B2] 2 110 010 011 000
    0xE6E0, // [33/B3] 3 111 011 111 000
    0xAE20, // [34/B4] 4 101 111 001 000
    0x64C0, // [35/B5] 5 011 010 110 000
    0x8EE0, // [36/B6] 6 100 111 111 000
    0xE220, // [37/B7] 7 111 001 001 000
    0x6EC0, // [38/B8] 8 011 111 110 000
    0xEE20, // [39/B9] 9 111 111 001 000
    0x4040, // [3A/BA] : 010 000 010 000
    0x0448, // [3B/BB] ; 000 010 010 100
    0x4840, // [3C/BC] < 010 100 010 000
    0xE0E0, // [3D/BD] = 111 000 111 000
    0x4240, // [3E/BE] > 010 001 010 000
    0x6240, // [3F/BF] ? 011 001 010 000

    0xCC20, // [40/C0] @ 110 110 001
    0x6996, // [41/C1] A : Circle 
    0x9669, // [42/C2] B : Circle R 
    0x6FF6, // [43/C3] C : Circle Filled
    0x8CEF, // [44/C4] D : R1 
    0xF731, // [45/C5] E : R2
    0xFEC8, // [46/C6] F : R3
    0x137F, // [47/C7] G : R4
    0x37FF, // [48/C8] H : Big Circle 1
    0xCEFF, // [49/C9] I : Big Circle 2
    0xFF73, // [4A/CA] J : Big Circle 3
    0xFFEC, // [4B/CB] K : Big Circle 4
    0xCC33, // [4C/CC] L : Medium Checkerboard
    0xCCCC, // [4D/CD] M : Half block LV
    0x00FF, // [4E/CE] N : Half block BH
    0x6666, // [4F/CF] O : Middle block V

    0x0FF0, // [50/D0] P : Middle block H
    0x0660, // [51/D1] Q : Small block
    0xA5A5, // [52/D2] R : Checkerboard
    0x8484, // [53/D3] S : Half Checkerboard LV
    0x2121, // [54/D4] T : Half Checkerboard RV
    0x00A5, // [55/D5] U : Half Checkerboard BH
    0xA500, // [56/D6] V : Half Checkerboard TH
    0xCE73, // [57/D7] W : Diagonal Left
    0x318C, // [58/D8] X : Diagonal Left R
    0x37EC, // [59/D9] Y : Diagonal Right
    0xC813, // [5A/DA] Z : Diagonal Right R
    0xC8C0, // [5B/DB] [ 110 100 110 000
    0x8420, // [5C/DC] \ 100 010 001 000
    0x6260, // [5D/DD] ] 011 001 011 000
    0x4A00, // [5E/DE] ^ 010 101 000 000
    0x00E0, // [5F/DF] _ 000 000 111 000

    0x8400, // [60/E0] ` 100 010 000 000
    0x4EA0, // [61/E1] A 000 010 110 000
    0xCEE0, // [62/E2] B 100 110 110 000
    0x6860, // [63/E3] C 000 110 110 000
    0xCAC0, // [64/E4] D 010 110 110 000
    0xECE0, // [65/E5] E 000 100 110 000
    0xEC80, // [66/E6] F 010 100 100 000
    0xCAE0, // [67/E7] G 000 110 110 110
    0xAEA0, // [68/E8] H 100 110 110 000
    0x4440, // [69/E9] I 000 100 100 000
    0x22C0, // [6A/EA] J 000 010 010 100
    0xACA0, // [6B/EB] K 100 110 101 000
    0x88E0, // [6C/EC] L 100 100 010 000
    0xEEA0, // [6D/ED] M 000 110 111 000
    0xEAA0, // [6E/EE] N 000 110 101 000
    0xEAE0, // [6F/EF] O 000 110 110 000

    0xEE80, // [70/F0] P 000 110 110 100
    0xEAC0, // [71/F1] Q 000 110 110 010
    0xCEA0, // [72/F2] R 000 110 100 000
    0x64C0, // [73/F3] S 000 010 100 000
    0xE440, // [74/F4] T 010 110 011 000
    0xAAE0, // [75/F5] U 000 101 111 000
    0xAA40, // [76/F6] V 000 101 010 000
    0xAEE0, // [77/F7] W 000 111 011 000
    0xA4A0, // [78/F8] X 000 110 110 000
    0xA440, // [79/F9] Y 000 101 111 001
    0xE4E0, // [7A/FA] Z 000 100 010 000
    0x6C60, // [7B/FB] { 011 110 011 000
    0x4444, // [7C/FC] | 010 010 010 010
    0xC6C0, // [7D/FD] } 110 011 110 000
    0x6C00, // [7E/FE] ~ 011 110 000 000
    0xA4A4  // [7F/FF]   101 010 101 010 (Middle Checkerboard)
};

typedef struct dise_t {
    ubyte status;
    /*
        1. Mode (G or T)
        2. Page 2 (T only)
        3. Convert on change
        4. 
        5. 
        6. 
        7. 
        8. 
    */
    uword offset: 12;
    ubyte width;
    ubyte height;
    ubyte frame_rate;
    uint foreground;
    uint background;
} dise_t;

// https://wiki.xxiivv.com/site/famicom.html
// #define DISE_OUT_CTRL   0b0000
// #define DISE_OUT_MASK   0b0001
// #define DISE_OUT_STATUS 0b0010
// #define DISE_OUT_ADDR   0b0011
// #define DISE_OUT_SCROLL 0b0100
// #define DISE_OUT_DATA   0b0101

#define DISE_IN_FLAGS       0b0000 // Получить статус в регистр RD

#define DISE_OUT_DATA       0b0010 // Отправить данные из регистра RС в адрес RA
// #define DISE_OUT_ADDRESS    0b0111 // Изменить указатель (берется из RA)
#define DISE_OUT_MODE       0b1000 // Изменить режим отображения (0: G / 1: T)
// #define DISE_OUT_STAT_RA    0b1100 // Занести статус устройства в регистр RA
// #define DISE_OUT_STAT_RB    0b1101 // Занести статус устройства в регистр RB
// #define DISE_OUT_STAT_RC    0b1110 // Занести статус устройства в регистр RC 
// #define DISE_OUT_STAT_RD    0b1111 // Занести статус устройства в регистр RD 
void dise_init(dise_t* dise, ubyte width, ubyte height, uword offset);
void dise_term(dise_t* dise);
void dise_render(dise_t* dise, ebvc_t* ebvc, void** pixels);

ubyte dise_input(dise_t* dise, ebvc_t* ebvc, ubyte mode);
void dise_output(dise_t* dise, ebvc_t* ebvc, ubyte mode);

#endif // DISE_H
