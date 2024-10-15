SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, obj/%.o, $(SRC))
OUT := enjhin

# Default to Linux settings
CC := cc

INCLUDE_PATH := ./include
LIB_PATH := ./lib

RAYLIB_INCLUDE_PATH := $(INCLUDE_PATH)/raylib-5.0
RAYLIB_LIB_PATH := $(LIB_PATH)/raylib-5.0/linux_amd64
JANSSON_INCLUDE_PATH := $(INCLUDE_PATH)/jansson-2.14
JANSSON_LIB_PATH := $(LIB_PATH)/jansson-2.14/linux_amd64

CFLAGS := -Wall
CFLAGS += -I$(INCLUDE_PATH)
CFLAGS += -I$(RAYLIB_INCLUDE_PATH)
CFLAGS += -I$(JANSSON_INCLUDE_PATH)

LDFLAGS := -L$(JANSSON_LIB_PATH) -Wl,-R$(JANSSON_LIB_PATH) -l:libjansson.a
LDFLAGS += -L$(RAYLIB_LIB_PATH) -Wl,-R$(RAYLIB_LIB_PATH) -l:libraylib.a -lm -lpthread

# Detect source and target OS
OS_FROM := linux
OS_FOR := linux

# Parse flags (e.g., make OS_FROM=linux OS_FOR=windows)
ifeq ($(OS_FOR), windows)
	OUT := $(OUT).exe
	RAYLIB_LIB_PATH := $(LIB_PATH)/raylib-5.0/win64_mingw-w64
	JANSSON_LIB_PATH := $(LIB_PATH)/jansson-2.14/win64_mingw-w64

	LDFLAGS := -L$(JANSSON_LIB_PATH) -Wl,-R$(JANSSON_LIB_PATH) -l:libjansson.a
	LDFLAGS += --static -L$(RAYLIB_LIB_PATH) -Wl,-R$(RAYLIB_LIB_PATH) -l:libraylib.a -l:libm.a -l:libpthread.a -l:libgdi32.a -l:libwinmm.a
ifeq ($(OS_FROM), linux)
        CC := x86_64-w64-mingw32-cc
    endif
endif

build: obj $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(LDFLAGS)
	@printf "\e[33mLinking\e[90m %s\e[0m\n" $@
	@printf "\e[34mDone!\e[0m\n"

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)
	@printf "\e[36mCompile\e[90m %s\e[0m\n" $@

obj:
	mkdir -p obj

clean:
	rm -f $(OUT) $(OUT).exe $(OBJ)
	@printf "\e[34mAll clear!\e[0m\n"
