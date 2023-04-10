# C config
CNAME 	= ebvc
CC 		= clang
CFLAGS 	= -std=c99 -g -Wall -O2
CLIB    = `sdl2-config --libs`
CINC    = `sdl2-config --cflags`
OUTPATH = .
SRCPATH = ./src
SOURCES = \
	$(SRCPATH)/main.c \
	$(SRCPATH)/ebvc.c \
	$(SRCPATH)/devi/syse.c

.PHONY: ebvc

all: ebvc
	@echo "Done!"

ebvc:
	$(CC) $(CFLAGS) $(CLIB) $(CINC) $(SOURCES) -o $(OUTPATH)/$(CNAME)
