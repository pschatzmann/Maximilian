/**
 * @file maxiMalloc.h
 * @author Phil Schatzmann
 * @brief malloc with psram support for ESP32 callable from C and CPP
 * @version 0.1
 * @date 2022-05-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _MAXI_MALLOC
#define _MAXI_MALLOC

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void * maxi_malloc(size_t size);

#ifdef __cplusplus
}
#endif

#endif