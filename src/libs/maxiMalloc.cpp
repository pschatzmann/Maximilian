/**
 * @file maxiMalloc.cpp
 * @author Phil Schatzmann
 * @brief PSRAM support for Arduino ESP32
**/

#include "maxiMalloc.h"
#include "assert.h"

#if defined(ESP32) && defined(ARDUINO)
#include "Arduino.h"

void* maxi_malloc(size_t size){
    void *result = nullptr;
    log_i("-> maxi_malloc: %d\n", size);
    result = ps_malloc(size);
    if (result != nullptr) return result;

    result = malloc(size);
    if (result == nullptr){
        log_e("Total PSRAM: %d", ESP.getPsramSize());
        log_e("Free PSRAM: %d", ESP.getFreePsram());
        log_e("Total heap: %d", ESP.getHeapSize());
        log_e("Free heap: %d", ESP.getFreeHeap());
        log_e("Max alloc heap: %d", ESP.getMaxAllocHeap());

        log_e("Error: malloc failed for size %d\n", size);
    }
    // print stack trace
    assert(result!=nullptr);
    return result;
}

#else

void* maxi_malloc(size_t size){
    void *result = nullptr;
    result = malloc(size);
    // print stack trace
    assert(result!=nullptr);
    return result;
}

#endif