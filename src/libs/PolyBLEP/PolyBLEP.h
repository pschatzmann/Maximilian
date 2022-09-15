#ifndef POLY_BLEP_H_INCLUDED
#define POLY_BLEP_H_INCLUDED

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

#ifdef CHEERP
#define CHEERP_EXPORT [[cheerp::jsexport]]
#include <cheerp/clientlib.h>
#else
#define CHEERP_EXPORT
#endif

#undef M_PI
#undef TWO_PI

class CHEERP_EXPORT PolyBLEP {
public:
    enum Waveform {
        SINE=0,
        COSINE,
        TRIANGLE,
        SQUARE,
        RECTANGLE,
        SAWTOOTH,
        RAMP,
        MODIFIED_TRIANGLE,
        MODIFIED_SQUARE,
        HALF_WAVE_RECTIFIED_SINE,
        FULL_WAVE_RECTIFIED_SINE,
        TRIANGULAR_PULSE,
        TRAPEZOID_FIXED,
        TRAPEZOID_VARIABLE
    };

    PolyBLEP(float sampleRate);

    // virtual ~PolyBLEP();

    void setFrequency(float freqInHz);

    void setSampleRate(float sampleRate);

    void setWaveform(Waveform waveform);

    void setPulseWidth(float pw);

    float get() const;

    void inc();

    float getAndInc();

    float getFreqInHz() const;

    void sync(float phase);

protected:
    const float M_PI = 3.14159265358979323846264338327950288f;
    const float TWO_PI = M_PI*2;
    Waveform waveform;
    float sampleRate;
    float freqInSecondsPerSample;
    float amplitude; // Frequency dependent gain [0.0..1.0]
    float pulseWidth; // [0.0..1.0]
    float t; // The current phase [0.0..1.0f) of the oscillator.

    void setdt(float time);

    float sin() const;

    float cos() const;

    float half() const;

    float full() const;

    float tri() const;

    float tri2() const;

    float trip() const;

    float trap() const;

    float trap2() const;

    float sqr() const;

    float sqr2() const;

    float rect() const;

    float saw() const;

    float ramp() const;
};


#endif
