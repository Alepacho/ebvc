#include "syse.h"

ubyte syse_input(ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        case SYSE_IN_BREAK: return 1;
        // case SYSE_IN_WRITE: {
        //     const char symbol = ebvc->reg[RC];
        //     // if (symbol == '\n') printf("NEW LINE!!!\n");
        //     printf("%c", symbol);
        //     // printf("symbol: '%c'\n", symbol);
        //     fflush(stdout);
        // } break;
    }

    return 0;
}

void syse_output(ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        default:
            printf("SYSE OUTPUT!\n");
    }
    exit(1);
}