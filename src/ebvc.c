#include "ebvc.h"

// Implementation
// * Boot
ebvc_result ebvc_init(ebvc_t* ebvc, uword ram_size) {
    // get the current calendar time
    srand((unsigned)(time(NULL) / 2));
    
    // if (ebvc != NULL) return NOT_EMPTY;
    // if (ebvc->ram != NULL) return NOT_EMPTY;
    ebvc->ram_size = ram_size;
    ebvc->ram = (ubyte*)malloc(ram_size * sizeof(ubyte));
    for (uword i = 0; i < ram_size; i++) ebvc->ram[i] = rand();

    // ebvc->speed  = 25000; // 2.5 MHz
    ebvc->working = true;
    ebvc->reg[0]  = 0;
    ebvc->reg[1]  = 0;
    ebvc->reg[2]  = 0;
    ebvc->reg[3]  = 0;
    ebvc->pc      = 0;
    ebvc->sr      = 0;
    ebvc->sp      = ram_size - 1;
    ebvc->v4      = 0;
    ebvc->v6      = 0;
    ebvc->input   = NULL;
    ebvc->output  = NULL;

    return SUCCESS;
}

// * ROM Loading
ebvc_result ebvc_load(ebvc_t* ebvc, const char* path) {
    if (ebvc == NULL) return NO_DEVICE;
    FILE *fp = NULL;
    fp = fopen(path ,"rb");

    if (fp != NULL) {
        fread(ebvc->ram, ebvc->ram_size * sizeof(ubyte), 1, fp);
        // fwrite(ebvc.ram, 4096 * sizeof(ubyte), 1, fp);
        fclose(fp);
        return SUCCESS;
    }

    return NO_FILE;
}

// * Execution
ebvc_result ebvc_eval(ebvc_t* ebvc) {
    if (ebvc == NULL) return NO_DEVICE;
    if (ebvc->working == false) return NOT_WORKING;
    // if (((ebvc->sr & 0b10000000) >> 7) == 1) return SYS_BREAK;

    const ubyte in = ebvc->ram[ebvc->pc];
    const ubyte opcode = (in & 0b11000000) >> 6; // always opcode
    const ubyte mode   = (in & 0b00110000) >> 4; // mode or reg
    const ubyte r1     = (in & 0b00001100) >> 2; // may be val
    const ubyte r2     = (in & 0b00000011)     ; // may be val
    // ebvc_debug(ebvc);

    switch (opcode) {
        case 0x00: {
            const ubyte d = (in & 0b00111111);
            if (d == 0)
                ebvc_eval_dio(ebvc); 
            else 
                ebvc_eval_jmp(ebvc, d);
        } break;
        case 0x01: {
            switch (mode) {
                case 0x00: {
                    if (r1 == r2) 
                        ebvc_eval_shl(ebvc, (REG)r1);
                    else
                        ebvc_eval_add(ebvc, (REG)r1, (REG)r2);
                 } break;
                case 0x01: {
                    switch (in & 0b00001111) {
                        case 0b0000: ebvc_eval_clr(ebvc); break;
                        case 0b0101: ebvc_eval_gsr(ebvc); break;
                        case 0b1010: ebvc_eval_pts(ebvc); break;
                        case 0b1111: ebvc_eval_pfs(ebvc); break;
                        default:
                            ebvc_eval_sub(ebvc, (REG)r1, (REG)r2);
                    }
                } break;
                case 0x02: ebvc_eval_lda(ebvc, (REG)r1, (REG)r2); break;
                case 0x03: ebvc_eval_sta(ebvc, (REG)r1, (REG)r2); break;
            }
        } break;
        case 0x02: {
            switch (mode) {
                case 0x00: ebvc_eval_beq(ebvc, (REG)r1, (REG)r2); break;
                case 0x01: ebvc_eval_bnq(ebvc, (REG)r1, (REG)r2); break;
                case 0x02: {
                    if (r1 == r2) {
                        ebvc_eval_dec(ebvc, (REG)r1);
                    } else ebvc_eval_slt(ebvc, (REG)r1, (REG)r2);
                } break;
                case 0x03: {
                    if (r1 == r2) {
                        ebvc_eval_inc(ebvc, (REG)r1);
                    } else ebvc_eval_smt(ebvc, (REG)r1, (REG)r2);
                 } break;
            }
        } break;
        case 0x03: ebvc_eval_set(ebvc, (REG)mode, r2 + (r1 << 2)); break;
        default:
            return UNKNOWN;
    }

    ebvc->pc++;
    return SUCCESS;
}

// * Power Off
ebvc_result ebvc_term(ebvc_t* ebvc) {
    if (ebvc == NULL) return NO_DEVICE;
    ebvc_debug(ebvc);

    free(ebvc->ram);
    ebvc->ram = NULL;
    return SUCCESS;
}

ebvc_result ebvc_set_pc(ebvc_t* ebvc, uword pc) {
    if (ebvc == NULL) return NO_DEVICE;
    ebvc->pc = pc & 0xFFF;
    return SUCCESS;
}

ebvc_result ebvc_set_input(ebvc_t* ebvc, ebvc_input input) {
    if (ebvc == NULL) return NO_DEVICE;
    ebvc->input = input;
    return SUCCESS;
}

ebvc_result ebvc_set_output(ebvc_t* ebvc, ebvc_output output) {
    if (ebvc == NULL) return NO_DEVICE;
    ebvc->output = output;
    return SUCCESS;
}

void ebvc_eval_add(ebvc_t* ebvc, REG r1, REG r2) { ebvc->reg[r1] += ebvc->reg[r2]; }
void ebvc_eval_shl(ebvc_t* ebvc, REG r1) { ebvc->reg[r1] = ebvc->reg[r1] << 1; } // same as r1 + r1
void ebvc_eval_sub(ebvc_t* ebvc, REG r1, REG r2) { ebvc->reg[r1] -= ebvc->reg[r2]; }
void ebvc_eval_clr(ebvc_t* ebvc) { for (ubyte i = 0; i < COUNT; i++) ebvc->reg[i] = 0; }
void ebvc_eval_gsr(ebvc_t* ebvc) { ebvc->reg[RD] = ebvc->sr; }
// TODO: add Stack overflow/underflow!
void ebvc_eval_pts(ebvc_t* ebvc) { ebvc->ram[ebvc->sp--] = ebvc->reg[RD]; } // push
void ebvc_eval_pfs(ebvc_t* ebvc) { ebvc->reg[RD] = ebvc->ram[++ebvc->sp]; } // pop
void ebvc_eval_lda(ebvc_t* ebvc, REG r1, REG r2) { ebvc->reg[r2] = ebvc->ram[ebvc->reg[r1]]; }
void ebvc_eval_sta(ebvc_t* ebvc, REG r1, REG r2) { ebvc->ram[ebvc->reg[r2]] = ebvc->reg[r1]; }
void ebvc_eval_jmp(ebvc_t* ebvc, uword address)  { ebvc->v6 = address; ebvc->pc += ebvc->v6; ebvc->pc--; }
void ebvc_eval_dio(ebvc_t* ebvc) { 
    const ubyte  reg = ebvc->reg[RD];
    const ubyte   io = (reg & 0b10000000) >> 7;
    const ubyte port = (reg & 0b01110000) >> 4;
    const ubyte mode =  reg & 0b00001111;
    if (io) {
        if (ebvc->debug_mode == 1) printf("input: %i, %i\n", port, mode);
        if (ebvc->input != NULL) ebvc->reg[RD] = ebvc->input(ebvc, port, mode);
    } else {
        if (ebvc->debug_mode == 1) printf("output: %i, %i\n", port, mode);
        if (ebvc->output != NULL) ebvc->output(ebvc, port, mode);
    }
}
void ebvc_eval_beq(ebvc_t* ebvc, REG r1, REG r2) { if (ebvc->reg[r1] == 0) { ebvc->pc += (sbyte)ebvc->reg[r2]; ebvc->pc--; } }
void ebvc_eval_bnq(ebvc_t* ebvc, REG r1, REG r2) { if (ebvc->reg[r1] != 0) { ebvc->pc += (sbyte)ebvc->reg[r2]; ebvc->pc--; } }
void ebvc_eval_slt(ebvc_t* ebvc, REG r1, REG r2) { ebvc->reg[r1] = ebvc->reg[r1] < ebvc->reg[r2]; }
void ebvc_eval_dec(ebvc_t* ebvc, REG r1) { ebvc->reg[r1]--; }
void ebvc_eval_smt(ebvc_t* ebvc, REG r1, REG r2) { ebvc->reg[r1] = ebvc->reg[r1] > ebvc->reg[r2]; }
void ebvc_eval_inc(ebvc_t* ebvc, REG r1) { ebvc->reg[r1]++; }
void ebvc_eval_set(ebvc_t* ebvc, REG reg, ubyte value) { ebvc->v4 = value; ebvc->reg[reg] = ebvc->v4; }

void ebvc_debug(ebvc_t* ebvc) {
    if (ebvc->debug_mode == 0) return;
    printf(" ********* DEBUG INFO ********* \n");
    
    uword bin = ebvc->pc;
    printf("PC    : 0b%d%d%d%d %d%d%d%d %d%d%d%d (%#05x)\n",
        (bin >> 11) & 1, (bin >> 10) & 1, (bin >> 9) & 1, (bin >> 8) & 1,
        (bin >>  7) & 1, (bin >>  6) & 1, (bin >> 5) & 1, (bin >> 4) & 1,
        (bin >>  3) & 1, (bin >>  2) & 1, (bin >> 1) & 1, (bin >> 0) & 1,
        bin
    );

    bin = ebvc->sp;
    printf("STACK : 0b%d%d%d%d %d%d%d%d %d%d%d%d (%#05x) : %i\n",
        (bin >> 11) & 1, (bin >> 10) & 1, (bin >> 9) & 1, (bin >> 8) & 1,
        (bin >>  7) & 1, (bin >>  6) & 1, (bin >> 5) & 1, (bin >> 4) & 1,
        (bin >>  3) & 1, (bin >>  2) & 1, (bin >> 1) & 1, (bin >> 0) & 1,
        bin, ebvc->ram[bin]
    );

    bin = ebvc->sr;
    printf("STATUS: 0b%d%d%d%d %d%d%d%d\n",
       (bin >> 7) & 1, (bin >> 6) & 1, (bin >> 5) & 1, (bin >> 4) & 1,
       (bin >> 3) & 1, (bin >> 2) & 1, (bin >> 1) & 1, (bin >> 0) & 1);

    bin = ebvc->ram[ebvc->pc];
    printf("INSTR : 0b%d%d%d%d %d%d%d%d\n",
       (bin >> 7) & 1, (bin >> 6) & 1, (bin >> 5) & 1, (bin >> 4) & 1,
       (bin >> 3) & 1, (bin >> 2) & 1, (bin >> 1) & 1, (bin >> 0) & 1);
    
    OPCODE op = ebvc_debug_opcode(ebvc, bin);
    ebvc_debug_data(ebvc, op, (bin & 0b00111111));

    printf("RA    : %03i (0x%02x)\n", ebvc->reg[0], ebvc->reg[0]);
    printf("RB    : %03i (0x%02x)\n", ebvc->reg[1], ebvc->reg[1]);
    printf("RC    : %03i (0x%02x)\n", ebvc->reg[2], ebvc->reg[2]);
    printf("RD    : %03i (0x%02x)\n", ebvc->reg[3], ebvc->reg[3]);
    printf("\n");
    printf("V4    : %03i (0x%08x)\n", ebvc->v4, ebvc->v4);
    printf("V6    : %03i (0x%08x)\n", ebvc->v6, ebvc->v6);


    printf("\n");
}

OPCODE ebvc_debug_opcode(ebvc_t* ebvc, ubyte opcode) {
    printf("OPCODE: ");
    OPCODE result = IDK;
    const ubyte l = (opcode & 0b11000000) >> 6;
    const ubyte r = (opcode & 0b00110000) >> 4;
    // ubyte m = 0b11110000;
    const ubyte r1 = (opcode & 0b00001100) >> 2;
    const ubyte r2 = (opcode & 0b00000011);
    switch (l) {
        case 0x00: {
            if ((opcode & 0b00111111) == 0) {
                result = DIO; printf("DIO"); 
            } else {
                result = JMP; printf("JMP"); 
            }
        } break;
        case 0x01: {
            switch (r) {
                case 0x00: {
                    if (r1 == r2) { result = SHL; printf("SHL"); } else { result = ADD; printf("ADD"); }
                } break;
                case 0x01: {
                    
                    if (r1 == r2) {
                        switch (r1) {
                            case 0b00: result = CLR; printf("CLR"); break;
                            case 0b01: result = GSR; printf("GSR"); break;
                            case 0b10: result = PTS; printf("PTS"); break;
                            case 0b11: result = PFS; printf("PFS"); break;
                        }
                    } else { result = SUB; printf("SUB"); }
                } break;
                case 0x02: result = LDA; printf("LDA"); break;
                case 0x03: result = STA; printf("STA"); break;
            }
        } break;
        case 0x02: {
            switch (r) {
                case 0x00: result = BEQ; printf("BEQ"); break;
                case 0x01: result = BNQ; printf("BNQ"); break;
                case 0x02: {
                    if (r1 == r2) {
                        result = DEC; printf("DEC");
                    } else { result = SLT; printf("SLT"); }
                } break;
                case 0x03: {
                    if (r1 == r2) {
                        result = INC; printf("INC");
                    } else { result = SMT; printf("SMT"); }
                }break;
            }
        } break; // 10
        case 0x03: result = SET; printf("SET"); break; // 11
        default: printf("IDK");
    }
    printf(" (%#04x)\n", opcode);
    return result;
}

void ebvc_debug_reg(ebvc_t* ebvc, REG reg) {
    switch (reg) {
        case RA: printf("RA"); break; // 00
        case RB: printf("RB"); break; // 01
        case RC: printf("RC"); break; // 10
        case RD: printf("RD"); break; // 11
        default: printf("??");
    }
}

void ebvc_debug_data(ebvc_t* ebvc, OPCODE opcode, ubyte data) {
    printf("DATA  : ");
    
    // printf("opl: %i", opl);
    switch (opcode) {
        case CLR: printf("..."); break;
        case GSR: printf("SR = %i;", ebvc->sr); break;
        case PTS: printf("RD = %i;", ebvc->reg[RD]); break;
        case PFS: printf("SP = %i;", ebvc->ram[ebvc->sp]); break;
        case SHL: case DEC: case INC: {
            const ubyte r = (data & 0b0011);
             ebvc_debug_reg(ebvc, (REG)r); printf(": %i;", ebvc->reg[r]);
        } break;
        case ADD: case SUB: 
        case LDA: case STA: 
        case SLT: case SMT: {
            const ubyte r1 = (data & 0b1100) >> 2; 
            const ubyte r2 = (data & 0b0011); 
            ebvc_debug_reg(ebvc, (REG)r1); printf(": %i, ", ebvc->reg[r1]);
            ebvc_debug_reg(ebvc, (REG)r2); printf(": %i;",  ebvc->reg[r2]);
        } break;
        case BEQ: case BNQ: {
            const ubyte r1 = (data & 0b1100) >> 2; 
            const ubyte r2 = (data & 0b0011); 
            ebvc_debug_reg(ebvc, (REG)r1); printf(": %i, ", ebvc->reg[r1]);
            ebvc_debug_reg(ebvc, (REG)r2); printf(": %i;",  (sbyte)ebvc->reg[r2]);
        } break;
        case DIO: printf("RD: %i;", ebvc->reg[RD]); break; // case DIO: printf("%i, %i, %i, %i;", ebvc->reg[RA], ebvc->reg[RB], ebvc->reg[RC], ebvc->reg[RD]); break;
        case JMP: { ebvc->v6 = data; printf("%i;", ebvc->v6); } break;
        case SET: {
            const ubyte reg = (data & 0b110000) >> 4; 
            ebvc->v4 = data & 0b1111; 
            ebvc_debug_reg(ebvc, (REG)reg);
            printf(" (%i) = %i;", ebvc->reg[reg], ebvc->v4); 
        } break;
        default : printf("...");
    }
    printf("\n");
}

ubyte ebvc_check(ebvc_t* ebvc, ebvc_result result) {
    
    unsigned char r = 0; // no good
    switch (result) {
        case SUCCESS: r = 1; break;
        case NOT_WORKING: break;
        // case SYS_BREAK: printf("SYS BREAK\n"); break;
        case NO_DEVICE:
            printf("ERROR at %#05x: No Device Found!\n", ebvc->pc);
        break;
        case NOT_EMPTY:
            printf("ERROR at %#05x: RAM is not empty! Clear it before any further manipulation!\n", ebvc->pc);
        break;
        case NO_FILE:
            printf("ERROR : File not Found!\n");
        break;
        default:
            printf("ERROR at %#05x: Unknown error!\n", ebvc->pc);
    }

    // if (r == 0 && result != SYS_BREAK) ebvc_debug(ebvc);

    return r;
}
