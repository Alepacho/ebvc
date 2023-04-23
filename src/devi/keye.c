#include "keye.h"

ubyte keye_input(keye_t* keye, ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        case KEYE_IN_PRESS_RA: 
            /* printf("input: RA %i (%i)\n", keye->keys[ebvc->reg[RA]], ebvc->reg[RA]); */ 
            return keye->keys[ebvc->reg[RA]]; break;
        case KEYE_IN_PRESS_RB: 
            /* printf("input: RB %i (%i)\n", keye->keys[ebvc->reg[RB]], ebvc->reg[RB]); */ 
            return keye->keys[ebvc->reg[RB]]; break;
        case KEYE_IN_PRESS_RC: 
            // printf("input: RC %i (%i)\n", keye->keys[ebvc->reg[RC]], ebvc->reg[RC]); 
            return keye->keys[ebvc->reg[RC]]; break;
        default:
            printf("KEYE.INPUT: unknown mode: %i\n", mode);
    }
    return 0;
}

void keye_output(keye_t* keye, ebvc_t* ebvc, ubyte mode) {
    switch (mode) {
        default:
            printf("KEYE.OUTPUT: unknown mode: %i\n", mode);
    }
}

void keye_init(keye_t* keye) {
    for (int i = 0; i < KEYE_KEYS_COUNT; i++) keye->keys[i] = 0;
}

void keye_update(keye_t* keye, ubyte *keys) {
    // if (keys[0x20]) printf("space: %i\n", keys[32]);
    // SDL_GetKeyFromScancode(SDL_Scancode scancode)
    for (int i = 0; i < KEYE_KEYS_COUNT; i++) keye->keys[i] = keys[i];
}

void keye_term(keye_t* keye) {

}


