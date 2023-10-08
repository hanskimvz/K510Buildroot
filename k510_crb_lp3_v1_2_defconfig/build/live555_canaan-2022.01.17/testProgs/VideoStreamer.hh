/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

//#define SYNC_WITH_ENC    1

#define BANK_SIZE 150000

int VideoStreamer_init(double framerate);
int VideoStreamer(long bufAddr, uint32_t bufSize, void *shared_vAddr);
int VideoStreamer_deinit();


