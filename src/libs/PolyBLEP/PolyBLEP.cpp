/*
PolyBLEP Waveform generator ported from the Jesusonic code by Tale
http://www.taletn.com/reaper/mono_synth/

Permission has been granted to release this port under the WDL/IPlug license:

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include "PolyBLEP.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <cmath>
#include <cstdint>

#ifdef ARDUINO
#define fmax fmax
#define fmin fmin
#else
#define fmax std::fmax
#define fmin std::fmin
#endif

#undef TWO_PI
#undef M_PI


template<typename T>
inline T square_number(const T &x) {
    return x * x;
}

// Adapted from "Phaseshaping Oscillator Algorithms for Musical Sound
// Synthesis" by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa
// Valimaki.
// http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/
inline float blep(float t, float dt) {
    if (t < dt) {
        return -square_number(t / dt - 1);
    } else if (t > 1 - dt) {
        return square_number((t - 1) / dt + 1);
    } else {
        return 0;
    }
}

// Derived from blep().
inline float blamp(float t, float dt) {
    if (t < dt) {
        t = t / dt - 1;
        return -1 / 3.0f * square_number(t) * t;
    } else if (t > 1 - dt) {
        t = (t - 1) / dt + 1;
        return 1 / 3.0f * square_number(t) * t;
    } else {
        return 0;
    }
}

template<typename T>
inline int64_t bitwiseOrZero(const T &t) {
    return static_cast<int64_t>(t) | 0;
}

PolyBLEP::PolyBLEP(float sampleRate)
        : sampleRate(sampleRate), amplitude(1.0f), t(0.0f) {
    setSampleRate(sampleRate);
    setFrequency(440.0f);
    setWaveform(SINE);
    setPulseWidth(0.5f);
}

void PolyBLEP::setdt(float time) {
    freqInSecondsPerSample = time;
}

void PolyBLEP::setFrequency(float freqInHz) {
    setdt(freqInHz / sampleRate);
}

void PolyBLEP::setSampleRate(float sampleRate) {
    const float freqInHz = getFreqInHz();
    this->sampleRate = sampleRate;
    setFrequency(freqInHz);
}

float PolyBLEP::getFreqInHz() const {
    return freqInSecondsPerSample * sampleRate;
}

void PolyBLEP::setPulseWidth(float pulseWidth) {
    this->pulseWidth = pulseWidth;
}

void PolyBLEP::sync(float phase) {
    t = phase;
    if (t >= 0) {
        t -= bitwiseOrZero(t);
    } else {
        t += 1 - bitwiseOrZero(t);
    }
}

void PolyBLEP::setWaveform(Waveform waveform) {
    this->waveform = waveform;
}

float PolyBLEP::get() const {
    if(getFreqInHz() >= sampleRate / 4) {
        return sin();
    } else switch (waveform) {
        case SINE:
            return sin();
        case COSINE:
            return cos();
        case TRIANGLE:
            return tri();
        case SQUARE:
            return sqr();
        case RECTANGLE:
            return rect();
        case SAWTOOTH:
            return saw();
        case RAMP:
            return ramp();
        case MODIFIED_TRIANGLE:
            return tri2();
        case MODIFIED_SQUARE:
            return sqr2();
        case HALF_WAVE_RECTIFIED_SINE:
            return half();
        case FULL_WAVE_RECTIFIED_SINE:
            return full();
        case TRIANGULAR_PULSE:
            return trip();
        case TRAPEZOID_FIXED:
            return trap();
        case TRAPEZOID_VARIABLE:
            return trap2();
        default:
            return 0.0f;
    }
}

void PolyBLEP::inc() {
    t += freqInSecondsPerSample;
    t -= bitwiseOrZero(t);
}

float PolyBLEP::getAndInc() {
    const float sample = get();
    inc();
    return sample;
}

float PolyBLEP::sin() const {
    return amplitude * std::sin(TWO_PI * t);
}

float PolyBLEP::cos() const {
    return amplitude * std::cos(TWO_PI * t);
}

float PolyBLEP::half() const {
    float t2 = t + 0.5f;
    t2 -= bitwiseOrZero(t2);

    float y = (t < 0.5f ? 2.0f * std::sin(TWO_PI * t) - 2.0f / M_PI : -2.0f / M_PI);
    y += TWO_PI * freqInSecondsPerSample * (blamp(t, freqInSecondsPerSample) + blamp(t2, freqInSecondsPerSample));

    return amplitude * y;
}

float PolyBLEP::full() const {
    float _t = this->t + 0.25f;
    _t -= bitwiseOrZero(_t);

    float y = 2.f * std::sin(M_PI * _t) - 4.f / M_PI;
    y += TWO_PI * freqInSecondsPerSample * blamp(_t, freqInSecondsPerSample);

    return amplitude * y;
}

float PolyBLEP::tri() const {
    float t1 = t + 0.25f;
    t1 -= bitwiseOrZero(t1);

    float t2 = t + 0.75f;
    t2 -= bitwiseOrZero(t2);

    float y = t * 4;

    if (y >= 3) {
        y -= 4;
    } else if (y > 1) {
        y = 2 - y;
    }

    y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

    return amplitude * y;
}

float PolyBLEP::tri2() const {
    float pulseWidth = fmax(0.0001, fmin(0.9999, this->pulseWidth));
    float t1 = t + 0.5f * pulseWidth;
    t1 -= bitwiseOrZero(t1);

    float t2 = t + 1 - 0.5f * pulseWidth;
    t2 -= bitwiseOrZero(t2);

    float y = t * 2;

    if (y >= 2 - pulseWidth) {
        y = (y - 2) / pulseWidth;
    } else if (y >= pulseWidth) {
        y = 1 - (y - pulseWidth) / (1 - pulseWidth);
    } else {
        y /= pulseWidth;
    }

    y += freqInSecondsPerSample / (pulseWidth - pulseWidth * pulseWidth) * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

    return amplitude * y;
}

float PolyBLEP::trip() const {
    float t1 = t + 0.75f + 0.5f * pulseWidth;
    t1 -= bitwiseOrZero(t1);

    float y;
    if (t1 >= pulseWidth) {
        y = -pulseWidth;
    } else {
        y = 4 * t1;
        y = (y >= 2 * pulseWidth ? 4 - y / pulseWidth - pulseWidth : y / pulseWidth - pulseWidth);
    }

    if (pulseWidth > 0) {
        float t2 = t1 + 1 - 0.5f * pulseWidth;
        t2 -= bitwiseOrZero(t2);

        float t3 = t1 + 1 - pulseWidth;
        t3 -= bitwiseOrZero(t3);
        y += 2 * freqInSecondsPerSample / pulseWidth * (blamp(t1, freqInSecondsPerSample) - 2 * blamp(t2, freqInSecondsPerSample) + blamp(t3, freqInSecondsPerSample));
    }
    return amplitude * y;
}

float PolyBLEP::trap() const {
    float y = 4 * t;
    if (y >= 3) {
        y -= 4;
    } else if (y > 1) {
        y = 2 - y;
    }
    y = fmax(-1, fmin(1, 2 * y));
    float t1 = t + 0.125f;
    t1 -= bitwiseOrZero(t1);

    float t2 = t1 + 0.5f;
    t2 -= bitwiseOrZero(t2);

    // Triangle #1
    y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

    t1 = t + 0.375f;
    t1 -= bitwiseOrZero(t1);

    t2 = t1 + 0.5f;
    t2 -= bitwiseOrZero(t2);

    // Triangle #2
    y += 4 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

    return amplitude * y;
}

float PolyBLEP::trap2() const {
    float pulseWidth = fmin(0.9999, this->pulseWidth);
    float scale = 1 / (1 - pulseWidth);

    float y = 4 * t;
    if (y >= 3) {
        y -= 4;
    } else if (y > 1) {
        y = 2 - y;
    }
    y = fmax(-1, fmin(1, scale * y));
    float t1 = t + 0.25f - 0.25f * pulseWidth;
    t1 -= bitwiseOrZero(t1);

    float t2 = t1 + 0.5f;
    t2 -= bitwiseOrZero(t2);

    // Triangle #1
    y += scale * 2 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

    t1 = t + 0.25f + 0.25f * pulseWidth;
    t1 -= bitwiseOrZero(t1);

    t2 = t1 + 0.5f;
    t2 -= bitwiseOrZero(t2);

    // Triangle #2
    y += scale * 2 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

    return amplitude * y;
}

float PolyBLEP::sqr() const {
    float t2 = t + 0.5f;
    t2 -= bitwiseOrZero(t2);

    float y = t < 0.5f ? 1 : -1;
    y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

    return amplitude * y;
}

float PolyBLEP::sqr2() const {
    float t1 = t + 0.875f + 0.25f * (pulseWidth - 0.5f);
    t1 -= bitwiseOrZero(t1);

    float t2 = t + 0.375f + 0.25f * (pulseWidth - 0.5f);
    t2 -= bitwiseOrZero(t2);

    // Square #1
    float y = t1 < 0.5f ? 1 : -1;

    y += blep(t1, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

    t1 += 0.5f * (1 - pulseWidth);
    t1 -= bitwiseOrZero(t1);

    t2 += 0.5f * (1 - pulseWidth);
    t2 -= bitwiseOrZero(t2);

    // Square #2
    y += t1 < 0.5f ? 1 : -1;

    y += blep(t1, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

    return amplitude * 0.5f * y;
}

float PolyBLEP::rect() const {
    float t2 = t + 1 - pulseWidth;
    t2 -= bitwiseOrZero(t2);

    float y = -2 * pulseWidth;
    if (t < pulseWidth) {
        y += 2;
    }

    y += blep(t, freqInSecondsPerSample) - blep(t2, freqInSecondsPerSample);

    return amplitude * y;
}

float PolyBLEP::saw() const {
    float _t = t + 0.5f;
    _t -= bitwiseOrZero(_t);

    float y = 2 * _t - 1;
    y -= blep(_t, freqInSecondsPerSample);

    return amplitude * y;
}

float PolyBLEP::ramp() const {
    float _t = t;
    _t -= bitwiseOrZero(_t);

    float y = 1 - 2 * _t;
    y += blep(_t, freqInSecondsPerSample);

    return amplitude * y;
}
