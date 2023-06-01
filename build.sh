#!/bin/bash

if [ -z "$1" ]
  then
    echo "-----------------------------------------------"
    echo "eg: build.sh path/to/project/dir"
    echo "-----------------------------------------------"
    exit
fi

#set -x

export PROJECT_PATH=$1
export PROJECT_NAME=$(cat $1/CMakeLists.txt  | grep 'project\(.+\)' -E | sed -E -n 's/.*\((.*)\).*$/\1/p')
export FIRMWARE_PATH=$PROJECT_PATH/build
export BIN_FILE=$PROJECT_PATH/build/$PROJECT_NAME.bin
export ESP_FILE=$PROJECT_PATH/build/$PROJECT_NAME.esp32

# BUILD EXECUTABLE

cd $1
idf.py build
cd ..

# FLASH EXECUTABLE

dd if=/dev/zero bs=1M count=4 of=$ESP_FILE
dd if="$FIRMWARE_PATH/bootloader/bootloader.bin" bs=1 count=$(stat -c%s "$FIRMWARE_PATH/bootloader/bootloader.bin") seek=$((16#1000)) conv=notrunc of=$ESP_FILE
dd if="$FIRMWARE_PATH/partition_table/partition-table.bin" bs=1 count=$(stat -c%s "$FIRMWARE_PATH/partition_table/partition-table.bin") seek=$((16#8000)) conv=notrunc of=$ESP_FILE
dd if=$BIN_FILE bs=1 count=$(stat -c%s "$BIN_FILE") seek=$((16#10000)) conv=notrunc of=$ESP_FILE

# START QEMU

./run.sh $ESP_FILE

