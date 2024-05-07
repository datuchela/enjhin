#!/bin/bash

SOURCE_FILE_PATH="./src/debug.c ./src/fizziks.c ./src/main.c"
OUTPUT_FILE_DIR=.
OUTPUT_FILE_NAME=game

COMPILER_BIN="gcc"
BUILD_FLAGS="-I./raylib-5.0 -L./raylib-5.0 -lraylib -lm -lpthread"

OS_FROM="Linux"
OS_FOR="Linux"

while getopts 'f:t:' FLAG; do
    case "$FLAG" in
        f)
            case "$OPTARG" in
                l|linux)
                    OS_FROM="Linux"
                    ;;
                w|windows)
                    OS_FROM="Windows"
                    COMPILER_BIN="gcc"
                    ;;
            esac
            ;;
        t)
            case "$OPTARG" in
                l|linux)
                    OS_FOR="Linux"
                    ;;
                w|windows)
                    OS_FOR="Windows"
                    OUTPUT_FILE_NAME=game.exe
                    BUILD_FLAGS="-I./raylib-5.0_win64_mingw-w64/include -L./raylib-5.0_win64_mingw-w64/lib -lraylib -lm -lpthread -lgdi32 -lwinmm"
                    if [ "$OS_FROM" == "Linux" ]; then
                        COMPILER_BIN="x86_64-w64-mingw32-gcc"
                    fi
                    ;;
            esac
            ;;
    esac
done

if !(test -d $OUTPUT_FILE_DIR); then
    mkdir $OUTPUT_FILE_DIR
fi

echo "---------------------------------"

echo "Building from $OS_FROM for $OS_FOR"
echo "⏳ Starting the build"
echo "Source: $SOURCE_FILE_PATH"
echo "Desired output: $OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME"
echo "COMPILER_BIN: $COMPILER_BIN"
echo "BUILD_FLAGS: $BUILD_FLAGS"
echo "---------------------------------"

$COMPILER_BIN $SOURCE_FILE_PATH -o $OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME $BUILD_FLAGS && \

    echo "✅ Build finished successfully!
Output path: $OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME" || \
    echo "❌ Something went wrong with the build"

echo "---------------------------------"
