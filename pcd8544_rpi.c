/*
=================================================================================
  Name        : pcd8544_rpi.c
  Version     : 0.1

  Copyright (C) 2012 by Andre Wussow, 2012, desk@binerry.de

  Description :
    A simple PCD8544 LCD (Nokia3310/5110) for Raspberry Pi for displaying some system informations.
    Makes use of WiringPI-library of Gordon Henderson (https://projects.drogon.net/raspberry-pi/wiringpi/)

    Recommended connection (http://www.raspberrypi.org/archives/384):
    LCD pins      Raspberry Pi
    LCD1 - GND    P06  - GND
    LCD2 - VCC    P01 - 3.3V
    LCD3 - CLK    P11 - GPIO0
    LCD4 - Din    P12 - GPIO1
    LCD5 - D/C    P13 - GPIO2
    LCD6 - CS     P15 - GPIO3
    LCD7 - RST    P16 - GPIO4
    LCD8 - LED    P01 - 3.3V 

================================================================================
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
================================================================================
 */
#include <wiringPi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include "PCD8544.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TEMP_FILE_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE 32

// pin setup
int _din = 1;
int _sclk = 0;
int _dc = 2;
int _rst = 4;
int _cs = 3;

// lcd contrast 
//may be need modify to fit your screen!  normal: 30- 90 ,default is:45 !!!maybe modify this value!
int contrast = 60;

int main(void) {
  // print infos
  printf("Raspberry Pi PCD8544 sysinfo display\n");
  printf("========================================\n");

  // check wiringPi setup
  if (wiringPiSetup() == -1) {
    printf("wiringPi-Error\n");
    exit(1);
  }

  // init and clear lcd
  LCDInit(_sclk, _din, _dc, _cs, _rst, contrast);
  LCDclear();

  // show logo
  LCDshowLogo();

  delay(2000);

  for (;;) {
    // clear lcd
    LCDclear();

    // get system usage / info
    struct sysinfo sys_info;
    if (sysinfo( & sys_info) != 0) {
      printf("sysinfo-Error\n");
    }

    // uptime
    char uptimeInfo[15];
    unsigned long uptime = sys_info.uptime / 60;
    sprintf(uptimeInfo, "Uptime %ld min.", uptime);

    // cpu info
    char cpuInfo[10];
    unsigned long avgCpuLoad = sys_info.loads[0] / 1000;
    sprintf(cpuInfo, "CPU  %ld%%", avgCpuLoad);

    // ram info
    char ramInfo[10];
    unsigned long totalRam = sys_info.freeram / 1024 / 1024;
    sprintf(ramInfo, "RAM  %ld MB", totalRam);

    // cpu Temperature
    char cputemperature[15];
    float cpu_temperature_Result = Get_Cpu_Temperature();
    sprintf(cputemperature, "TEMP %.2f C", cpu_temperature_Result);

    // build screen
    //LCDdrawstring(0, 0, "Raspberry Pi:");
    LCDdrawstring(0, 0, "SunFounder.com");
    LCDdrawline(0, 10, 83, 10, BLACK);
    LCDdrawstring(0, 12, uptimeInfo);
    LCDdrawstring(0, 20, cpuInfo);
    LCDdrawstring(0, 28, ramInfo);
    LCDdrawstring(0, 36, cputemperature);
    LCDdisplay();

    delay(1000);
  }

  //for (;;){
  //  printf("LED On\n");
  //  digitalWrite(pin, 1);
  //  delay(250);
  //  printf("LED Off\n");
  //  digitalWrite(pin, 0);
  //  delay(250);
  //}

  return 0;
}

int Get_Cpu_Temperature(void) {
  int fd;
  double temp = 0;
  char buf[MAX_SIZE];

  // open/sys/class/thermal/thermal_zone0/temp 
  fd = open(TEMP_FILE_PATH, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "failed to open thermal_zone0/temp\n");
    return -1;
  }

  // read temperature
  if (read(fd, buf, MAX_SIZE) < 0) {
    fprintf(stderr, "failed to read temp\n");
    return -1;
  }

  // print as float 
  temp = atoi(buf) / 1000.0;

  // close file
  close(fd);
  return temp;
}