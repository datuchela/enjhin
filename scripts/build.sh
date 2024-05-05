#!/bin/bash

SOURCE_FILE_PATH="./src/fizziks.c ./src/main.c"
OUTPUT_FILE_DIR=.
OUTPUT_FILE_NAME=game
OUTPUT_FILE_PATH=$OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME
BUILD_FLAGS="-I./raylib-5.0 -L./raylib-5.0 -lraylib -lm -lpthread"
COMPILER_BIN="gcc"

echo "---------------------------------"
if [ "$1" == "win64" ]; then
    OUTPUT_FILE_NAME=game.exe
    COMPILER_BIN="x86_64-w64-mingw32-gcc"
    BUILD_FLAGS="-I./raylib-5.0_win64_mingw-w64/include -L./raylib-5.0_win64_mingw-w64/lib -lraylib -lm -lpthread -lgdi32 -lwinmm"
    echo "Building for (Win64)"
fi

echo "⏳ Starting the build"
echo "Source: $SOURCE_FILE_PATH"
echo "Desired output: $OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME"
echo "---------------------------------"

if !(test -d $OUTPUT_FILE_DIR); then
    mkdir $OUTPUT_FILE_DIR
fi

$COMPILER_BIN $SOURCE_FILE_PATH \
    -o $OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME \
    $BUILD_FLAGS && \

    echo "✅ Build finished successfully!
Output path: $OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME" || \
    echo "❌ Something went wrong with the build"
echo "---------------------------------"
