#!/bin/bash

RELEASE_NAME="enjhin-$(date +%F)"
BINARY_PATH="./game"
RELEASES_FOLDER_PATH="./releases"

while getopts 'w' FLAG; do
    case "$FLAG" in
        w)
            RELEASE_NAME="$RELEASE_NAME"_win64
            BINARY_PATH="./game.exe"
            ;;
    esac
done

echo "RELEASE_NAME: $RELEASE_NAME"
echo "BINARY_PATH: $BINARY_PATH"

mkdir $RELEASES_FOLDER_PATH/$RELEASE_NAME && cp -r assets $RELEASES_FOLDER_PATH/$RELEASE_NAME/ && mv $BINARY_PATH $RELEASES_FOLDER_PATH/$RELEASE_NAME/ && cd $RELEASES_FOLDER_PATH && zip -r $RELEASE_NAME.zip $RELEASE_NAME
