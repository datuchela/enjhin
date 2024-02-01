#!/bin/bash

SOURCE_FILE_PATH=./src/main.c
OUTPUT_FILE_PATH=./dist/game

echo '⏳ Starting the build'
echo "Source: $SOURCE_FILE_PATH"
echo "Output: $OUTPUT_FILE_PATH"

gcc $SOURCE_FILE_PATH \
    -o $OUTPUT_FILE_PATH \
    -I. -L./lib -lraylib -lm && \

    echo "✅ Build finished successfully!
Output path: $OUTPUT_FILE_PATH" || \
    echo '❌ Something went wrong with the build'
