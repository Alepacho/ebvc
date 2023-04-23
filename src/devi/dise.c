#include "dise.h"

#include <stdlib.h>
#include <math.h>

int interpolate(int color1, int color2, float fraction) {
        ubyte r1 = (color1 >> 16) & 0xff;
        ubyte r2 = (color2 >> 16) & 0xff;
        ubyte g1 = (color1 >>  8) & 0xff;
        ubyte g2 = (color2 >>  8) & 0xff;
        ubyte b1 = color1 & 0xff;
        ubyte b2 = color2 & 0xff;

        return  (sint) ((r2 - r1) * fraction + r1) << 16 |
                (sint) ((g2 - g1) * fraction + g1) <<  8 |
                (sint) ((b2 - b1) * fraction + b1);
}

ubyte dise_input(dise_t* dise, ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        default: printf("DISE.OUTPUT: unknown mode: %i\n", mode); break;
    }
    return 0;
}

void dise_output(dise_t* dise, ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        case DISE_OUT_MODE: {
            dise->status = (dise->status ^ 0b10000000); 
        } break;
        case DISE_OUT_DATA: {
            ebvc->ram[dise->offset + ebvc->reg[RA]] = ebvc->reg[RC];
        } break;
        default: printf("DISE.OUTPUT: unknown mode: %i\n", mode); break;
    }
} 

void dise_init(dise_t* dise, ubyte width, ubyte height, uword offset) {
    dise->status = 0b00000000;
    // dise->status = 0b10000000;
    dise->offset = offset;
    dise->width  = width;
    dise->height = height;
    dise->frame_rate = 30;
    dise->foreground = 0xFFAA00;
    dise->background = 0x101010;
    printf("SCREEN SIZE  : %i bytes.\n", dise->width * dise->height / 8);
    // printf("SCREEN SCALE : %i\n", dise->scale);
    printf("SCREEN OFFSET: %i\n", dise->offset);
}

void dise_term(dise_t* dise) {

}

void dise_render(dise_t* dise, ebvc_t* ebvc, void** pixels) {
    const int w = dise->width;
    const int h = dise->height;
    const int b = 8;
    const int r = rand() % 2 ? 0x030300 : 0;
    const int c = fmax(dise->foreground - r, 0);

    const bool text_mode = (dise->status & 0b10000000) >> 7;
    if (text_mode) {
        const ubyte tw = 16;
        const ubyte th = 8;
        for (int y = 0; y < th; y++) {
            for (int x = 0; x < tw; x++) {
                const ubyte d = ebvc->ram[dise->offset + (x + y * tw)];
                const bool invert = (d & 0b10000000) >> 7;
                const uword f = dise_font[(d & 0b01111111)];
                for (int i = 0; i < 4; i++) {
                    const int ii = (3 - i) * 4;
                    ubyte p = ((f & (0x000F << ii)) >> ii);
                    // ubyte p = ((f & 0xF000) >> (i * 4));
                    
                    for (int j = 0; j < 4; j++) {
                        const uint a = (x * 4 + y * 4 * w) + i * w + j;
                        ((int*)pixels)[a] = 
                            interpolate(((int*)pixels)[a], 
                            ((p & (0b1000 >> j))) ? 
                                (invert ? dise->background : c) : 
                                (invert ? c : dise->background), 
                            dise->frame_rate / 60.0
                        );
                    }
                }
            }
        }
        // exit(1);
    } else {
        for (int i = 0; i < w * h; i += b) {
            const ubyte p = ebvc->ram[dise->offset + i / b];
            for (int j = 0; j < b; j++) ((int*)pixels)[i + j] = 
                interpolate(((int*)pixels)[i + j], 
                (p & (0b10000000 >> j)) ? c : dise->background, 
                // (p & (0b00000001 << j)) >> j ? c : dise->background, 
                dise->frame_rate / 60.0
            );
                // ? interpolate(((int*)pixels)[i + j], c, dise->frame_rate / 60.0) 
                // : interpolate(((int*)pixels)[i + j], dise->background, dise->frame_rate / 60.0);
        }
    }
}
