SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, obj/%.o, $(SRC))
OUT := enjhin

# Default to Linux settings
CC := cc
INCLUDE_PATH := ./raylib/raylib-5.0_linux_amd64/include
LIB_PATH := ./raylib/raylib-5.0_linux_amd64/lib

CFLAGS := -Wall -I$(INCLUDE_PATH)
LDFLAGS := -L$(LIB_PATH) -Wl,-R$(LIB_PATH) -l:libraylib.a -lm -lpthread

# Detect source and target OS
OS_FROM := linux
OS_FOR := linux

# Parse flags (e.g., make OS_FROM=linux OS_FOR=windows)
ifeq ($(OS_FOR), windows)
	OUT := $(OUT).exe
	INCLUDE_PATH := ./raylib/raylib-5.0_win64_mingw-w64/include
	LIB_PATH := ./raylib/raylib-5.0_win64_mingw-w64/lib
	CFLAGS := -Wall -I$(INCLUDE_PATH)
	LDFLAGS := -L$(LIB_PATH) -Wl,-R$(LIB_PATH) -lraylib -lm -lpthread -lgdi32 -lwinmm
ifeq ($(OS_FROM), linux)
        CC := x86_64-w64-mingw32-gcc
    endif
endif

build: obj $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)
	@printf "\e[33mLinking\e[90m %s\e[0m\n" $@
	@printf "\e[34mDone!\e[0m\n"

obj/%.o: src/%.c raylib
	$(CC) -c $< -o $@ $(CFLAGS) $(CFLAGS)
	@printf "\e[36mCompile\e[90m %s\e[0m\n" $@

obj:
	mkdir -p obj

clean:
	rm -f $(OUT) $(OUT).exe $(OBJ)
	@printf "\e[34mAll clear!\e[0m\n"
