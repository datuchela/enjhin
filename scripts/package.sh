#!/bin/bash

RELEASE_NAME="enjhin-$(date +%F)"
BINARY_PATH="./game"
RELEASES_DIR_PATH="./releases"

while getopts 'w' FLAG; do
    case "$FLAG" in
        w)
            RELEASE_NAME="$RELEASE_NAME"_win64
            BINARY_PATH="./game.exe"
            ;;
        *)
            echo "PACKAGING FOR LINUX"
    esac
done

echo "RELEASE_NAME: $RELEASE_NAME"
echo "BINARY_PATH: $BINARY_PATH"

if ! test -d $RELEASES_DIR_PATH; then
    mkdir $RELEASES_DIR_PATH
fi

mkdir $RELEASES_DIR_PATH/"$RELEASE_NAME" \
    && cp -r assets $RELEASES_DIR_PATH/"$RELEASE_NAME"/ \
    && mv $BINARY_PATH $RELEASES_DIR_PATH/"$RELEASE_NAME"/ \
    && cd $RELEASES_DIR_PATH \
    && zip -r "$RELEASE_NAME".zip "$RELEASE_NAME"
