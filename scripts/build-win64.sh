#!/bin/bash

SOURCE_FILE_PATH="./src/fizziks.c ./src/main.c"
OUTPUT_FILE_DIR=.
OUTPUT_FILE_NAME=game.exe
OUTPUT_FILE_PATH=$OUTPUT_FILE_DIR/$OUTPUT_FILE_NAME

echo "⏳ Starting the build (Win64)"
echo "Source: $SOURCE_FILE_PATH"
echo "Output: $OUTPUT_FILE_PATH"

if !(test -d $OUTPUT_FILE_DIR); then
    mkdir $OUTPUT_FILE_DIR
fi

x86_64-w64-mingw32-gcc $SOURCE_FILE_PATH \
    -o $OUTPUT_FILE_PATH \
    -I./raylib-5.0_win64_mingw-w64/include -L./raylib-5.0_win64_mingw-w64/lib -lraylib -lm -lpthread -lgdi32 -lwinmm && \

    echo "✅ Build finished successfully!
Output path: $OUTPUT_FILE_PATH" || \
    echo "❌ Something went wrong with the build"
