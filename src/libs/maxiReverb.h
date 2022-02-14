/*
 Coded by Tom Rushmore,
 http:///github.com/tomrushmore
 */
/*
 *  platform independent synthesis library using portaudio or rtaudio
 *
 *  Created by Mick Grierson on 29/12/2009.
 *  Copyright 2009 Mick Grierson & Strangeloop Limited. All rights reserved.
 *	Thanks to the Goldsmiths Creative Computing Team.
 *	Special thanks to Arturo Castro for the PortAudio implementation.
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without
 *	restriction, including without limitation the rights to use,
 *	copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following
 *	conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *	OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __maxiReverb__
#define __maxiReverb__

#include "../maximilian.h"
#include <valarray>

class maxiReverbFilters{
public:
    maxiReverbFilters();
    maxi_float_t twopoint(maxi_float_t input);
    maxi_float_t comb1(maxi_float_t input,maxi_float_t size);
    maxi_float_t combff(maxi_float_t input,maxi_float_t size);
    maxi_float_t combfb(maxi_float_t input,maxi_float_t size,maxi_float_t fb);
    maxi_float_t lpcombfb(maxi_float_t input,maxi_float_t size,maxi_float_t fb, maxi_float_t cutoff);

    maxi_float_t allpass(maxi_float_t input,maxi_float_t size);
    maxi_float_t allpass(maxi_float_t input,maxi_float_t size,maxi_float_t fback);
    maxi_float_t allpasstap(maxi_float_t input,maxi_float_t size,int tap);
    void setlength(int length);
    maxi_float_t onetap(maxi_float_t input,maxi_float_t size);
    maxi_float_t tapd(maxi_float_t input,maxi_float_t size, maxi_float_t * taps,int numtaps);
    maxi_float_t tapdwgain(maxi_float_t input,maxi_float_t size, maxi_float_t * taps,int numtaps,maxi_float_t * gain);
    maxi_float_t tapdpos(maxi_float_t input,int size, int * taps,int numtaps);
    maxi_float_t gettap(int tap);

private:
    std::valarray<maxi_float_t> delay_line;
    maxi_float_t a;
    int delay_index;
    int delay_size;
    maxi_float_t output;
    maxi_float_t feedback;
    maxi_float_t gain_cof;

    maxiFilter mf;


};

class maxiReverbBase{
public:
    maxiReverbBase();

protected:
    maxi_float_t parallellpcomb(maxi_float_t input,int firstfilter,int numfilters);
    void setcombtimesms(maxi_float_t times[],int numset);
    void setaptimesms(maxi_float_t times[],int numset);
    maxi_float_t serialallpass(maxi_float_t input,int firstfilter,int numfilters);
    maxi_float_t serialallpass(maxi_float_t input,int firstfilter,int numfilters,maxi_float_t fb);
    maxi_float_t parallelcomb(maxi_float_t input,int firstfilter, int numfilters);
    maxi_float_t apcombcombo(maxi_float_t input,maxi_float_t gain_coef);

    maxi_float_t fbsignal[8];
    void setweights(maxi_float_t weights[],int numset,maxi_float_t *filter);
    int mstodellength(maxi_float_t ms);
    void setcombtimes(int times[],int numset);
    void setaptimes(int times[],int numset);
    void setcombweights(maxi_float_t weights[],int numset);
    void setcombweightsall(maxi_float_t feedback);
    void setapweights(maxi_float_t weights[],int numset);
    void setlpcombcutoff(maxi_float_t *cutoff, int numset);
    void setlpcombcutoffall(maxi_float_t cutoff);
    void setcombfeedback(maxi_float_t *feedback,int numset);
    void limitnumfilters(int * num);

    static int const numfilters = 32;
    maxiReverbFilters fArrayAllP[numfilters];
    maxiReverbFilters fArrayTwo[numfilters];
    maxiFilter fArrayLP[numfilters];


    maxi_float_t fbcomb[numfilters];
    maxi_float_t fbap[numfilters];
    maxi_float_t combgainweight[numfilters];
    maxi_float_t apgainweight[numfilters];
    maxi_float_t lpcombcutoff[numfilters];
    maxi_float_t feedbackcombfb[numfilters];
    maxi_float_t output;
    maxi_float_t stereooutput[2];
    maxi_float_t accumulator;
    float numsamplesms;

    maxiReverbFilters earlyref;
    maxi_float_t taps[numfilters];
    maxi_float_t tapsgain[numfilters];
    int numtaps;
    int tapdellength;
};

class maxiSatReverb : private maxiReverbBase {
public:
    maxiSatReverb();
    maxi_float_t play(maxi_float_t input);
    maxi_float_t* playStereo(maxi_float_t input);

};

class maxiFreeVerb : private maxiReverbBase {
public:
    maxiFreeVerb();
    maxi_float_t play(maxi_float_t input);
    maxi_float_t play(maxi_float_t input,maxi_float_t roomsize,maxi_float_t absorbtion);
};

class maxiFreeVerbStereo : private maxiReverbBase {
public:
    maxiFreeVerbStereo();
    maxi_float_t* playStereo(maxi_float_t input,maxi_float_t roomsize,maxi_float_t absorbtion);
};

class maxiDattaroReverb : private maxiReverbBase {
public:
    maxiDattaroReverb();
    maxi_float_t* playStereo(maxi_float_t input);
private:
    maxiReverbFilters maxiDelays[9];
    static const int numdattarotappos = 14;
    static const int numdattarotaps = 14;

    int dattarotapspos[numdattarotappos];
    maxi_float_t dattorotap[numdattarotaps];
    int maxideltimes[4];
    maxi_float_t dattorogains[5];
    int dattarofixdellengths[5];
    maxi_float_t sigl,sigr;

};


#endif /* defined(__maximilianZone__maxiReverb__) */
