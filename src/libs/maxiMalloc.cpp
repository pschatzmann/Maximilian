/**
 * @file maxiMalloc.cpp
 * @author Phil Schatzmann
 * @brief PSRAM support for Arduino ESP32
**/

#include "maxiMalloc.h"

#if defined(ESP32) && defined(ARDUINO)
#include "Arduino.h"
#define MAXI_PSRAM_LIMIT 1024 

void* maxi_malloc(size_t size){
    void *result = nullptr;
    if (size >= MAXI_PSRAM_LIMIT && ESP.getPsramSize()>0) {
        // fprintf(stdout, "-> maxi_malloc: psram %d\n", size);
        result = ps_malloc(size);
        if (result==nullptr){
            fprintf(stderr, "Error: ps_malloc failed for size %d - available %d - total %d\n", size, ESP.getFreePsram(), ESP.getPsramSize());
        }
        // print stack trace
        assert(result!=nullptr);
        return result;
    }

    // fprintf(stdout, "-> maxi_malloc: %d\n", size);
    result = malloc(size);
    if (result==nullptr){
        fprintf(stderr, "Error: malloc failed for size %d\n", size);
    }
    // print stack trace
    assert(result!=nullptr);
    return result;
}

#else

void* maxi_malloc(size_t size){
    return malloc(size);
}

#endif