#include "syse.h"

ubyte syse_input(ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        default:
            printf("SYSE.INPUT: unknown mode: %i\n", mode);
    }

    return 0;
}

void syse_output(ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        case SYSE_OUT_BREAK: ebvc->working = false; break;
        case SYSE_OUT_WRITE: {
            const char symbol = ebvc->reg[RC];
            printf("%c", symbol);
            fflush(stdout);
        } break;
        default:
            printf("SYSE.OUTPUT: unknown mode: %i\n", mode);
    }
}
