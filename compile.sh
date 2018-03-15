#!/bin/bash
gcc -o app pcd8544_rpi.c PCD8544.c -L/usr/local/lib -lwiringPi
