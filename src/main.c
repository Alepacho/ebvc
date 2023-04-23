#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ebvc.h"
#include "devi/syse.h"
#include "devi/dise.h"
#include "devi/keye.h"

#include "SDL2/SDL.h"

/*
Memory Map:
= 0x000 .. 0x0FF =================== ZERO PAGE =
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00

00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
= 0x100 .. 0x1FF ===================== DISPLAY =
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00

00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
= 0x200 .. 0xF80 ======================== CODE =
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................

.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
.......................  .......................
= 0xF80 .. 0xFFF ======================= STACK =
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
= 0x1000 ================================= END =
*/

#define SW 64
#define SH 32
#define MEM_SIZE 4096
typedef struct emu_t {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    unsigned long now, last, frame;
    bool step_mode;
    bool keydown_enter;
    int scale;
} emu_t;

emu_t emu;
ebvc_t ebvc;
dise_t dise;
keye_t keye;
char* filepath = NULL;

void emu_stop() {
    ebvc.working = false;
    ebvc_debug(&ebvc);
}

void emu_error(const char* msg, const char* info) {
    printf("%s %s\n", msg, info);
    emu_stop();
}

void emu_init() {
    printf("\nEMU INIT\n");
    emu.scale = 13;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) emu_error("SDL", "Failed to init!");
    emu.window = SDL_CreateWindow("EBVC", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SW * emu.scale, SH * emu.scale,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (emu.window == NULL) emu_error("SDL", "Failed to create window!");
    emu.renderer = SDL_CreateRenderer(emu.window, -1, SDL_RENDERER_ACCELERATED);
    if (emu.renderer == NULL) emu_error("SDL", "Failed to create renderer!");
    emu.texture = SDL_CreateTexture(emu.renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SW, SH);
    if (emu.texture == NULL) emu_error("SDL", "Failed to create texture!");
}

void emu_term() {
    printf("\nEMU TERM\n");
    SDL_DestroyTexture(emu.texture);
    SDL_DestroyRenderer(emu.renderer);
    SDL_DestroyWindow(emu.window);
    SDL_Quit();
}

void emu_render() {
    void *pixels = NULL;
    int pitch;
    if (SDL_LockTexture(emu.texture, NULL, &pixels, &pitch)) emu_error("SDL", "Can't lock texture!");
    dise_render(&dise, &ebvc, pixels);
    SDL_UnlockTexture(emu.texture);

    SDL_SetRenderDrawColor(emu.renderer, 0, 0, 0, 255);
    // SDL_SetRenderDrawColor(emu.renderer, 255, 96, 128, 255);
    SDL_RenderClear(emu.renderer);

    int WW, WH;
    SDL_GetWindowSize(emu.window, &WW, &WH);
    int RATIO = fmin((float)WW * 0.99 / (float)SW, (float)WH * 0.99 / (float)(SH));
    SDL_Rect spos;
    spos.x = (int)(WW - SW * RATIO) / 2; spos.w = (int)(SW * RATIO);
    spos.y = (int)(WH - (SH) * RATIO) / 2; spos.h = (int)((SH) * RATIO);
    
    if (SDL_RenderCopy(emu.renderer, emu.texture, NULL, &spos)) emu_error("SDL", "Unable to render copy!");
    SDL_Rect viewport = { 0, 0, WW, WH };
    SDL_RenderSetViewport(emu.renderer, &viewport);
    SDL_RenderPresent(emu.renderer);
}

void emu_update() {
    emu.keydown_enter = false;
    const unsigned long interval = SDL_GetPerformanceFrequency() / 60;
    emu.frame = emu.now + interval;

    SDL_Event event;
    // bool keys[256];

    // Uint8* keys = (Uint8*)SDL_GetKeyboardState(NULL);
    bool keys[KEYE_KEYS_COUNT] = {0};
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: emu_stop(); break;
            case SDL_WINDOWEVENT: {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) emu_stop();
            } break;
            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_RETURN) emu.keydown_enter = true;
                // printf("key: %i\n", event.key.keysym.sym);
                if (event.key.keysym.sym >= 0 
                &&  event.key.keysym.sym < 256) keys[event.key.keysym.sym] = true;
            } break;
            default: ;
        }
    }
    
    // const unsigned char* keys = 
    keye_update(&keye, (ubyte*)keys);

    emu_render();

    emu.last = emu.now;
    emu.now = SDL_GetPerformanceCounter();
    
    const unsigned long ms = SDL_GetPerformanceFrequency() / 1000;
    if ((Sint64)(emu.frame - emu.now) > 0) {
        SDL_Delay((emu.frame - emu.now) / ms);
		emu.now = emu.frame;
        // printf("delain: %lu\n", now);
    }
}

void emu_save() {
    FILE *fp = NULL;
    fp = fopen("./saved.rom" ,"wb");
    if (fp != NULL) {
        fwrite(ebvc.ram, MEM_SIZE * sizeof(ubyte), 1, fp);
        fclose(fp);
    }
}

void emu_parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-h", 2) == 0) {
            printf("usage: ebvc [-h] [-l FILE] [-s SCALE] [-d[s]]   \n\n");
            printf("options:                                          \n");
            printf("    -h         show this help message             \n");
            printf("    -l FILE    load .rom file                     \n");
            printf("    -s SCALE   set window scale (default is 10)   \n");
            printf("    -d (-ds)   debug mode (+ step mode)           \n");
            printf("               (press enter to follow PC register)\n");
            exit(0);
        } else if (strncmp(argv[i], "-l", 2) == 0) {
            if (i + 1 < argc) filepath = argv[++i]; else printf("No File...\n");
        } else if (strncmp(argv[i], "-s", 2) == 0) {
            if (i + 1 < argc) emu.scale = atoi(argv[++i]); else printf("No Scale...\n");
        } else if (strncmp(argv[i], "-d", 2) == 0) {
            ebvc.debug_mode = 1;

            if (strncmp(argv[i], "-ds", 3) == 0) {
                printf("DEBUG MODE : ENABLED\n");
                emu.step_mode = true;
            }
        } else {
            printf("Unknown argument: %s\n", argv[i]);
        }
    }
}

// что-то получить из устройства (receive / get)
ubyte emu_ebvc_input(ebvc_t* ebvc, ubyte port, ubyte mode) {
    switch (port) {
        case 0b000: return syse_input(ebvc, mode);          break;
        case 0b001: return dise_input(&dise, ebvc, mode);   break;
        case 0b010: return keye_input(&keye, ebvc, mode);   break;
    }
    return rand() % 256;
}

// что-то отправить в устройство (send / set)
void emu_ebvc_output(ebvc_t* ebvc, ubyte port, ubyte mode) {
    switch (port) {
        case 0b000: syse_output(ebvc, mode);        break;
        case 0b001: dise_output(&dise, ebvc, mode); break;
        case 0b010: keye_output(&keye, ebvc, mode); break;
    }
}

int main(int argc, char* argv[]) {
    emu_init();
    if (argc > 1) emu_parse(argc, argv);

    assert(ebvc_check(&ebvc, ebvc_init(&ebvc, MEM_SIZE)));
    dise_init(&dise, SW, SH, 0x100);
    keye_init(&keye);
    if (filepath != NULL) ebvc.working = ebvc_check(&ebvc, ebvc_load(&ebvc, filepath));
    ebvc_set_pc(&ebvc, 0x200);
    ebvc_set_input(&ebvc,  (ebvc_input)emu_ebvc_input);
    ebvc_set_output(&ebvc, (ebvc_output)emu_ebvc_output);
    // emu_save();

    while (ebvc.working) {
        bool eval = true;
        emu_update();
        if (!ebvc.working) { emu_stop(); break; }

        if (emu.step_mode) {
            eval = false;
            if (emu.keydown_enter) { eval = true; }
        }

        if (eval) {
            ebvc_debug(&ebvc);
            if (!ebvc_check(&ebvc, ebvc_eval(&ebvc))) ebvc.working = false;
        }
    }

    keye_term(&keye);
    dise_term(&dise);
    ebvc_term(&ebvc);
    emu_term();
    return 0;
}
