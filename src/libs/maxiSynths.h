//
//  maxiSynths.h
//  granular
//
//  Created by Michael Grierson on 16/08/2015.
//

#ifndef __granular__maxiSynths__
#define __granular__maxiSynths__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include "math.h"
#include "../maximilian.h"


class maxiKick {
public:
    maxiKick();
    maxi_float_t play();
    void trigger();
    // void setPitch(maxi_float_t pitch);
    void setRelease(maxi_float_t releaseD);

    void setPitch(maxi_float_t v) {pitch = v;}
    maxi_float_t getPitch() const {return pitch;}
    void setDistortion(maxi_float_t v) {distortion = v;}
    maxi_float_t getDistortion() const {return distortion;}
    void setCutoff(maxi_float_t v) {cutoff = v;}
    maxi_float_t getCutoff()  const{return cutoff;}
    void setResonance(maxi_float_t v) {resonance = v;}
    maxi_float_t getResonance() const {return resonance;}

    void setUseDistortion(bool v) {useDistortion = v;}
    maxi_float_t getUseDistortion()  const {return useDistortion;}
    void setUseLimiter(bool v) {useLimiter = v;}
    maxi_float_t getUseLimiter()  const {return useLimiter;}
    void setUseFilter(bool v) {useFilter = v;}
    maxi_float_t getUseFilter()  const {return useFilter;}


  private:
    maxi_float_t pitch;
    maxi_float_t output = 0 ;
    maxi_float_t outputD =0 ;
    maxi_float_t envOut;
    bool useDistortion = false;
    bool useLimiter = false;
    bool useFilter = false;
    maxi_float_t distortion = 0;
    bool inverse = false;
    maxi_float_t cutoff;
    maxi_float_t resonance;
    maxi_float_t gain = 1;
    maxiOsc kick;
    maxiEnv envelope;
    maxiDistortion distort;
    maxiFilter filter;
};

class maxiSnare {
public:
    maxiSnare();
    maxi_float_t play();
    void setPitch(maxi_float_t pitch);
    void setRelease(maxi_float_t releaseD);
    void trigger();
    maxi_float_t pitch;
    maxi_float_t output = 0 ;
    maxi_float_t outputD = 0 ;
    maxi_float_t envOut;
    bool useDistortion = false;
    bool useLimiter = false;
    bool useFilter = true;
    maxi_float_t distortion = 0;
    bool inverse = false;
    maxi_float_t cutoff;
    maxi_float_t resonance;
    maxi_float_t gain = 1;
    maxiOsc tone;
    maxiOsc noise;
    maxiEnv envelope;
    maxiDistortion distort;
    maxiFilter filter;



};

class maxiHats {

public:
    maxiHats();
    maxi_float_t play();
    void setPitch(maxi_float_t pitch);
    void setRelease(maxi_float_t releaseD);
    void trigger();
    maxi_float_t pitch;
    maxi_float_t output = 0;
    maxi_float_t outputD = 0;
    maxi_float_t envOut;
    bool useDistortion = false;
    bool useLimiter = false;
    bool useFilter = false;
    maxi_float_t distortion = 0;
    bool inverse = false;
    maxi_float_t cutoff;
    maxi_float_t resonance;
    maxi_float_t gain = 1;
    maxiOsc tone;
    maxiOsc noise;
    maxiEnv envelope;
    maxiDistortion distort;
    maxiSVF filter;


};


class maxiSynth {



};


class granularSynth {



};


class maxiSampler {

public:
    maxiSampler();
    maxi_float_t play();
    void setPitch(maxi_float_t pitch, bool setall=false);
    void midiNoteOn(maxi_float_t pitch, maxi_float_t velocity, bool setall=false);
    void midiNoteOff(maxi_float_t pitch, maxi_float_t velocity, bool setall=false);
    void setAttack(maxi_float_t attackD,bool setall=true);
    void setDecay(maxi_float_t decayD,bool setall=true);
    void setSustain(maxi_float_t sustainD,bool setall=true);
    void setRelease(maxi_float_t releaseD,bool setall=true);
    void setPosition(maxi_float_t positionD,bool setall=true);
    void load(string inFile,bool setall=true);
    void setNumVoices(int numVoices);
    maxi_float_t position;
    void trigger();
    maxi_float_t pitch[32];
    int originalPitch=67;
    maxi_float_t outputs[32];
    maxi_float_t outputD = 0;
    maxi_float_t envOut[32];
    maxi_float_t envOutGain[32];
    maxi_float_t output;
    bool useDistortion = false;
    bool useLimiter = false;
    bool useFilter = false;
    maxi_float_t distortion = 0;
    bool inverse = false;
    maxi_float_t cutoff;
    maxi_float_t resonance;
    maxi_float_t gain = 1;
    int voices;
    int currentVoice=0;
    convert mtof;
    maxiOsc LFO1;
    maxiOsc LFO2;
    maxiOsc LFO3;
    maxiOsc LFO4;
    maxiSample samples[32];
    maxiEnv envelopes[32];
    maxiDistortion distort;
    maxiSVF filters[32];
    bool sustain = true;


};



#endif
