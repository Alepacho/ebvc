#ifndef EBVC_H
#define EBVC_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <stdbool.h>
// typedef unsigned  char bool;
// #define true  1
// #define false 0

typedef unsigned  char ubyte;
typedef   signed  char sbyte;
typedef unsigned short uword;
typedef   signed short sword;
typedef unsigned   int uint;
typedef   signed   int sint;

typedef enum RESULT {
    SUCCESS,
    NO_DEVICE,
    NOT_EMPTY,
    NO_FILE,
    NOT_WORKING,
    UNKNOWN
} ebvc_result;

typedef enum REG {
    RA, RB, RC, RD, COUNT
} REG;

typedef enum OPCODE {
    DIO, JMP,
    ADD, SHL,
    SUB, CLR, 
    GSR, PTS, 
    PFS, LDA, 
    STA, BEQ, 
    BNQ, SLT, 
    DEC, SMT, 
    INC, SET,
    IDK // UNKNOWN OPCODE
} OPCODE;

struct EBVC;

typedef ubyte(*ebvc_input) (struct EBVC* ebvc, ubyte port, ubyte mode); // what ebvc will send to device  (data from reg)
typedef void (*ebvc_output)(struct EBVC* ebvc, ubyte port, ubyte mode); // what ebvc will get from device (data to   reg)

typedef struct EBVC {
    // uint speed;
    bool working;
    bool  debug_mode;
    uword ram_size;
    ubyte *ram;
    ubyte reg[4];
    ubyte sr; // status register
    uword pc : 12;
    uword sp : 12;
    sbyte v4 :  4; // [-8  ..  7] used for set instruction
    sbyte v6 :  6; // [-32 .. 31] used for jmp instruction
    // status register bits: (draft)
    // 0 bit: Stack Overflow    (sp > 256)
    // 1 bit: Stack Underflow   (sp < 0)
    // 2 bit: Carry             (add/sub)
    // 3 bit: Zero              (beq/bnq/slt/smt)
    // 4 bit:  
    // 5 bit:  
    // 6 bit:  
    // 7 bit:  

    ebvc_input  input;
    ebvc_output output;
} ebvc_t;

// Declaration
ebvc_result ebvc_init(ebvc_t* ebvc, uword ram_size);
ebvc_result ebvc_load(ebvc_t* ebvc, const char* path);
ebvc_result ebvc_eval(ebvc_t* ebvc);
ebvc_result ebvc_term(ebvc_t* ebvc);

ebvc_result ebvc_set_pc(ebvc_t* ebvc, uword pc);
ebvc_result ebvc_set_input(ebvc_t* ebvc, ebvc_input input);
ebvc_result ebvc_set_output(ebvc_t* ebvc, ebvc_output output);

void ebvc_eval_add(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_shl(ebvc_t* ebvc, REG r1);
void ebvc_eval_sub(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_clr(ebvc_t* ebvc);
void ebvc_eval_gsr(ebvc_t* ebvc);
void ebvc_eval_pts(ebvc_t* ebvc);
void ebvc_eval_pfs(ebvc_t* ebvc);
void ebvc_eval_lda(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_sta(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_jmp(ebvc_t* ebvc, uword address);
void ebvc_eval_dio(ebvc_t* ebvc);
void ebvc_eval_beq(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_bnq(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_slt(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_dec(ebvc_t* ebvc, REG r1);
void ebvc_eval_smt(ebvc_t* ebvc, REG r1, REG r2);
void ebvc_eval_inc(ebvc_t* ebvc, REG r1);
void ebvc_eval_set(ebvc_t* ebvc, REG reg, ubyte value);

void   ebvc_debug(ebvc_t* ebvc);
OPCODE ebvc_debug_opcode(ebvc_t* ebvc, ubyte opcode);
void   ebvc_debug_reg(ebvc_t* ebvc, REG reg);
void   ebvc_debug_data(ebvc_t* ebvc, OPCODE op, ubyte data);
ubyte  ebvc_check(ebvc_t* ebvc, ebvc_result result);

#endif // EBVC_H
