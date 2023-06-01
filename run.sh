#!/bin/bash

export ESP_FILE=$1

qemu-system-xtensa -nographic -M esp32 -m 4 -drive file=$ESP_FILE,if=mtd,format=raw -gdb tcp::1234,nowait -serial pty -S
