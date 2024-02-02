#!/bin/bash

SOURCE_FILE_PATH=./src/main.c
OUTPUT_FILE_DIR=./dist
OUTPUT_FILE_PATH=$OUTPUT_FILE_DIR/game

echo '⏳ Starting the build'
echo "Source: $SOURCE_FILE_PATH"
echo "Output: $OUTPUT_FILE_PATH"

if !(test -d $OUTPUT_FILE_DIR); then
    mkdir $OUTPUT_FILE_DIR
fi

gcc $SOURCE_FILE_PATH \
    -o $OUTPUT_FILE_PATH \
    -I. -L./lib -lraylib -lm -ldl -lpthread && \

    echo "✅ Build finished successfully!
Output path: $OUTPUT_FILE_PATH" || \
    echo '❌ Something went wrong with the build'
