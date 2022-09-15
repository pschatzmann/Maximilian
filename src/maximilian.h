/*
*  platform independent synthesis library using portaudio or rtaudio
 *  maximilian.h
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

#ifndef MAXIMILIAN_H
#define MAXIMILIAN_H

//#define MAXIMILIAN_PORTAUDIO
#define MAXIMILIAN_RT_AUDIO

#include "maximilian_types.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include "math.h"
#include <cmath>
#include <vector>
#include <cfloat>
#ifdef _WIN32 //|| _WIN64
#include <algorithm>
#endif
#include <numeric>
#include "libs/maxiMalloc.h"

using namespace std;

#undef PI
#undef TWOPI

#define PI 3.1415926535897932384626433832795f
#define TWOPI 6.283185307179586476925286766559f

//transpiling some functions to Javascript?
#ifdef CHEERP
#define CHEERP_EXPORT [[cheerp::jsexport]]
#include <cheerp/clientlib.h>
#else
#define CHEERP_EXPORT
#endif

//if using CHEERP, then convert incoming arrays to vectors inplace, otherwise, preserve the C++ vector interface
//see maxiIndex for a simple example
#ifdef CHEERP

#define maxi_number_tARRAY_REF client::Float64Array *

#define maxi_number_tARRAY client::Float64Array 

inline vector<maxi_float_t> convertArrayFromJS(maxi_number_tARRAY_REF x)
{
    maxi_float_t *__arrayStart = __builtin_cheerp_make_regular<maxi_float_t>(x, 0);
    size_t __arrayLength = x->get_length();
    return vector<maxi_float_t>(__arrayStart, __arrayStart + __arrayLength);
}

#define NORMALISE_ARRAY_TYPE(invar, outvar) vector<maxi_float_t> outvar = convertArrayFromJS(invar);

#elif defined(USE_MAXVECTOR)

#define maxi_number_tARRAY_REF maxi_vector &
#define maxi_number_tARRAY maxi_vector
#define NORMALISE_ARRAY_TYPE(invar, outvar) maxi_vector outvar; outvar.normalize(invar);

#else

#define maxi_number_tARRAY_REF vector<maxi_float_t> &
#define maxi_number_tARRAY vector<maxi_float_t> 
#define NORMALISE_ARRAY_TYPE(invar, outvar) vector<maxi_float_t> outvar = vector<maxi_float_t>(invar.begin(), invar.end()); //emplace into new variable

#endif

const maxi_float_t pitchRatios[256] = {0.0006517771980725, 0.0006905338959768, 0.0007315951515920, 0.0007750981021672, 0.0008211878011934, 0.0008700182079338, 0.0009217521874234, 0.0009765623835847, 0.0010346318595111, 0.0010961542138830, 0.0011613349197432, 0.0012303915573284, 0.0013035543961450, 0.0013810677919537, 0.0014631903031841, 0.0015501962043345, 0.0016423756023869, 0.0017400364158675, 0.0018435043748468, 0.0019531247671694, 0.0020692637190223, 0.0021923084277660, 0.0023226698394865, 0.0024607831146568, 0.0026071087922901, 0.0027621355839074, 0.0029263808391988, 0.0031003924086690, 0.0032847514376044, 0.0034800728317350, 0.0036870087496936, 0.0039062497671694, 0.0041385274380445, 0.0043846168555319, 0.0046453396789730, 0.0049215662293136, 0.0052142175845802, 0.0055242711678147, 0.0058527616783977, 0.0062007848173380, 0.0065695028752089, 0.0069601456634700, 0.0073740174993873, 0.0078124995343387, 0.0082770548760891, 0.0087692337110639, 0.0092906802892685, 0.0098431324586272, 0.0104284351691604, 0.0110485423356295, 0.0117055233567953, 0.0124015696346760, 0.0131390057504177, 0.0139202913269401, 0.0147480349987745, 0.0156249990686774, 0.0165541097521782, 0.0175384692847729, 0.0185813605785370, 0.0196862649172544, 0.0208568722009659, 0.0220970865339041, 0.0234110467135906, 0.0248031392693520, 0.0262780115008354, 0.0278405826538801, 0.0294960699975491, 0.0312499981373549, 0.0331082195043564, 0.0350769385695457, 0.0371627211570740, 0.0393725298345089, 0.0417137444019318, 0.0441941730678082, 0.0468220934271812, 0.0496062822639942, 0.0525560230016708, 0.0556811690330505, 0.0589921437203884, 0.0624999962747097, 0.0662164390087128, 0.0701538771390915, 0.0743254423141479, 0.0787450596690178, 0.0834274888038635, 0.0883883461356163, 0.0936441868543625, 0.0992125645279884, 0.1051120460033417, 0.1113623380661011, 0.1179842874407768, 0.1249999925494194, 0.1324328780174255, 0.1403077542781830, 0.1486508846282959, 0.1574901193380356, 0.1668549776077271, 0.1767766922712326, 0.1872883737087250, 0.1984251290559769, 0.2102240920066833, 0.2227246761322021, 0.2359685748815536, 0.2500000000000000, 0.2648657560348511, 0.2806155085563660, 0.2973017692565918, 0.3149802684783936, 0.3337099552154541, 0.3535533845424652, 0.3745767772197723, 0.3968502581119537, 0.4204482138156891, 0.4454493522644043, 0.4719371497631073, 0.5000000000000000, 0.5297315716743469, 0.5612310171127319, 0.5946035385131836, 0.6299605369567871, 0.6674199104309082, 0.7071067690849304, 0.7491535544395447, 0.7937005162239075, 0.8408964276313782, 0.8908987045288086, 0.9438742995262146, 1.0000000000000000, 1.0594631433486938, 1.1224620342254639, 1.1892070770263672, 1.2599210739135742, 1.3348398208618164, 1.4142135381698608, 1.4983071088790894, 1.5874010324478149, 1.6817928552627563, 1.7817974090576172, 1.8877485990524292, 2.0000000000000000, 2.1189262866973877, 2.2449240684509277, 2.3784141540527344, 2.5198421478271484, 2.6696796417236328, 2.8284270763397217, 2.9966142177581787, 3.1748020648956299, 3.3635857105255127, 3.5635950565338135, 3.7754974365234375, 4.0000000000000000, 4.2378525733947754, 4.4898481369018555, 4.7568287849426270, 5.0396842956542969, 5.3393597602844238, 5.6568546295166016, 5.9932284355163574, 6.3496046066284180, 6.7271714210510254, 7.1271901130676270, 7.5509948730468750, 8.0000000000000000, 8.4757051467895508, 8.9796962738037109, 9.5136575698852539, 10.0793685913085938, 10.6787195205688477, 11.3137092590332031, 11.9864568710327148, 12.6992092132568359, 13.4543428421020508, 14.2543802261352539, 15.1019897460937500, 16.0000000000000000, 16.9514102935791016, 17.9593944549560547, 19.0273151397705078, 20.1587371826171875, 21.3574390411376953, 22.6274185180664062, 23.9729137420654297, 25.3984184265136719, 26.9086875915527344, 28.5087604522705078, 30.2039794921875000, 32.0000000000000000, 33.9028205871582031, 35.9187889099121094, 38.0546302795410156, 40.3174743652343750, 42.7148780822753906, 45.2548370361328125, 47.9458274841308594, 50.7968368530273438, 53.8173751831054688, 57.0175209045410156, 60.4079589843750000, 64.0000076293945312, 67.8056411743164062, 71.8375778198242188, 76.1092605590820312, 80.6349563598632812, 85.4297561645507812, 90.5096740722656250, 95.8916625976562500, 101.5936737060546875, 107.6347503662109375, 114.0350418090820312, 120.8159179687500000, 128.0000152587890625, 135.6112823486328125, 143.6751556396484375, 152.2185211181640625, 161.2699127197265625, 170.8595123291015625, 181.0193481445312500, 191.7833251953125000, 203.1873474121093750, 215.2695007324218750, 228.0700836181640625, 241.6318511962890625, 256.0000305175781250, 271.2225646972656250, 287.3503112792968750, 304.4370422363281250, 322.5398254394531250, 341.7190246582031250, 362.0386962890625000, 383.5666503906250000, 406.3746948242187500, 430.5390014648437500, 456.1401977539062500, 483.2637023925781250, 512.0000610351562500, 542.4451293945312500, 574.7006225585937500, 608.8740844726562500, 645.0796508789062500, 683.4380493164062500, 724.0773925781250000, 767.1333007812500000, 812.7494506835937500, 861.0780029296875000, 912.2803955078125000, 966.5274047851562500, 1024.0001220703125000, 1084.8903808593750000, 1149.4012451171875000, 1217.7481689453125000, 1290.1593017578125000, 1366.8762207031250000, 1448.1549072265625000, 1534.2666015625000000, 1625.4989013671875000};

class CHEERP_EXPORT maxiSettings
{
public:
    maxiSettings();
    static int sampleRate;
    static int channels;
    static int bufferSize;
    static void setup(int initSampleRate, int initChannels, int initBufferSize)
    {
        maxiSettings::sampleRate = initSampleRate;
        maxiSettings::channels = initChannels;
        maxiSettings::bufferSize = initBufferSize;
    }
    //
    // static void setSampleRate(int sampleRate_){
    // 	maxiSettings::sampleRate = sampleRate_;
    // }
    //
    // static void setNumChannels(int channels_){
    // 	maxiSettings::channels = channels_;
    // }
    //
    // static void setBufferSize(int bufferSize_){
    // 	maxiSettings::bufferSize = bufferSize_;
    // }
    //
    static int getSampleRate()
    {
        return maxiSettings::sampleRate;
    }
    //
    // static int getNumChannels() {
    // 	return maxiSettings::channels;
    // }
    //
    // static int getBufferSize() {
    // 	return maxiSettings::bufferSize;
    // }
};

class CHEERP_EXPORT maxiOsc
{

    maxi_float_t frequency;
    maxi_float_t phase;
    maxi_float_t startphase;
    maxi_float_t endphase;
    maxi_float_t output;
    maxi_float_t tri;

public:
    maxiOsc();
    maxi_float_t sinewave(maxi_float_t frequency);
    maxi_float_t coswave(maxi_float_t frequency);
    maxi_float_t phasor(maxi_float_t frequency);
    maxi_float_t phasorBetween(maxi_float_t frequency, maxi_float_t startphase, maxi_float_t endphase); //renamed to avoid overrides
    maxi_float_t saw(maxi_float_t frequency);
    maxi_float_t triangle(maxi_float_t frequency);
    maxi_float_t square(maxi_float_t frequency);
    maxi_float_t pulse(maxi_float_t frequency, maxi_float_t duty);
    maxi_float_t impulse(maxi_float_t frequency);
    maxi_float_t noise();
    maxi_float_t sinebuf(maxi_float_t frequency);
    maxi_float_t sinebuf4(maxi_float_t frequency);
    maxi_float_t sawn(maxi_float_t frequency);
    maxi_float_t rect(maxi_float_t frequency, maxi_float_t duty);
    void phaseReset(maxi_float_t phaseIn);
};

class maxiEnvelope
{

    maxi_float_t period;
    maxi_float_t output;
    maxi_float_t startval;
    maxi_float_t currentval;
    maxi_float_t nextval;
    int isPlaying;

public:
    maxiEnvelope() {}
    //	maxi_float_t line(int numberofsegments,maxi_float_t segments[100]);
    maxi_float_t line(int numberofsegments, std::vector<maxi_float_t> &segments);

    void trigger(int index, maxi_float_t amp);
    int valindex;
    maxi_float_t amplitude;

    // ------------------------------------------------
    // getters/setters
    void setValindex(int index)
    {
        valindex = index;
    }

    void setAmplitude(maxi_float_t amp)
    {
        amplitude = amp;
    }

    int getValindex() const
    {
        return valindex;
    }

    maxi_float_t getAmplitude() const
    {
        return amplitude;
    }
    // ------------------------------------------------
};


class CHEERP_EXPORT maxiDelayline
{
    maxi_float_t frequency;
    int phase;
    maxi_float_t startphase;
    maxi_float_t endphase;
    maxi_float_t output;
//    maxi_float_t memory[88200 * 2];
    maxi_float_t *memory=nullptr;
    const int memory_size = 88200; 

public:
    maxiDelayline();
    ~maxiDelayline() { if(memory!=nullptr) free(memory);}
    maxi_float_t dl(maxi_float_t input, int size, maxi_float_t feedback);
    maxi_float_t dlFromPosition(maxi_float_t input, int size, maxi_float_t feedback, int position); //renamed to avoid overrides
    void setupMemory();
};

class CHEERP_EXPORT maxiFilter
{
private:
    maxi_float_t gain;
    maxi_float_t input;
    maxi_float_t output;
    maxi_float_t inputs[10];
    maxi_float_t outputs[10];
    maxi_float_t cutoff1;
    maxi_float_t x; //speed
    maxi_float_t y; //pos
    maxi_float_t z; //pole
    maxi_float_t c; //filter coefficient

public:
    maxiFilter();
    maxi_float_t cutoff;
    maxi_float_t resonance;
    maxi_float_t lores(maxi_float_t input, maxi_float_t cutoff1, maxi_float_t resonance);
    maxi_float_t hires(maxi_float_t input, maxi_float_t cutoff1, maxi_float_t resonance);
    maxi_float_t bandpass(maxi_float_t input, maxi_float_t cutoff1, maxi_float_t resonance);
    maxi_float_t lopass(maxi_float_t input, maxi_float_t cutoff);
    maxi_float_t hipass(maxi_float_t input, maxi_float_t cutoff);

    // ------------------------------------------------
    // getters/setters
    void setCutoff(maxi_float_t cut)
    {
        cutoff = cut;
    }

    void setResonance(maxi_float_t res)
    {
        resonance = res;
    }

    maxi_float_t getCutoff()
    {
        return cutoff;
    }

    maxi_float_t getResonance()
    {
        return resonance;
    }
    // ------------------------------------------------
};

class maxiMix
{
    maxi_float_t input;
    maxi_float_t two[2];
    maxi_float_t four[4];
    maxi_float_t eight[8];

public:
    //	maxi_float_t x;
    //	maxi_float_t y;
    //	maxi_float_t z;

    // ------------------------------------------------
    // getters/setters

    // ------------------------------------------------

    //	maxi_float_t *stereo(maxi_float_t input,maxi_float_t two[2],maxi_float_t x);
    //	maxi_float_t *quad(maxi_float_t input,maxi_float_t four[4], maxi_float_t x,maxi_float_t y);
    //	maxi_float_t *ambisonic(maxi_float_t input,maxi_float_t eight[8],maxi_float_t x,maxi_float_t y, maxi_float_t z);

    // should return or just be void function
    void stereo(maxi_float_t input, std::vector<maxi_float_t> &two, maxi_float_t x);
    void quad(maxi_float_t input, std::vector<maxi_float_t> &four, maxi_float_t x, maxi_float_t y);
    void ambisonic(maxi_float_t input, std::vector<maxi_float_t> &eight, maxi_float_t x, maxi_float_t y, maxi_float_t z);
};

//lagging with an exponential moving average
//a lower alpha value gives a slower lag
template <class T>
class maxiLagExp
{
public:
    T alpha, alphaReciprocal;
    T val;

    maxiLagExp()
    {
        init(0.5, 0.0f);
    };

    maxiLagExp(T initAlpha, T initVal)
    {
        init(initAlpha, initVal);
    }

    void init(T initAlpha, T initVal)
    {
        alpha = initAlpha;
        alphaReciprocal = 1.0f - alpha;
        val = initVal;
    }

    inline void addSample(T newVal)
    {
        val = (alpha * newVal) + (alphaReciprocal * val);
    }

    // getters/setters
    void setAlpha(T alpha_)
    {
        alpha = alpha_;
    }

    void setAlphaReciprocal(T alphaReciprocal_)
    {
        alphaReciprocal = alphaReciprocal_;
    }

    void setVal(T val_)
    {
        val = val_;
    }

    T getAlpha() const
    {
        return alpha;
    }

    T getAlphaReciprocal() const
    {
        return alphaReciprocal;
    }

    inline T value() const
    {
        return val;
    }
};

class CHEERP_EXPORT maxiTrigger
{
public:
    maxiTrigger();
    //zerocrossing
    maxi_float_t onZX(maxi_float_t input)
    {
        maxi_float_t isZX = 0.0f;
        if ((previousValue <= 0.0f || firstTrigger) && input > 0)
        {
            isZX = 1.0;
        }
        previousValue = input;
        firstTrigger = 0;
        return isZX;
    }

    //change detector
    maxi_float_t onChanged(maxi_float_t input, maxi_float_t tolerance)
    {
        maxi_float_t changed = 0;
        if (fabs(input - previousValue) > tolerance)
        {
            changed = 1;
        }
        previousValue = input;
        return changed;
    }

private:
    maxi_float_t previousValue = 1;
    bool firstTrigger = 1;
};

// class DualModeF64Array {

// public:
// #ifdef CHEERP
//     client::Float64Array *data = new client::Float64Array(1);
// #else
//     std::vector<maxi_float_t> data;
// #endif

//     size_t size() {
// #ifdef CHEERP
//         return data->get_length();
// #else
//         return data.size();
// #endif

//     }

//     void clear() {
// #ifdef CHEERP
//         data->fill(0);
// #else
//         data.clear();
// #endif

//     }

// #ifdef CHEERP
//     void setFrom(client::Float64Array *newdata) {
//         data = new client::Float64Array(newdata);
//     }
// #else
//     void setFrom(std::vector<maxi_float_t> &newdata) {
//         data = newdata;
//     }
// #endif



// };
#ifdef CHEERP
#define DECLARE_F64_ARRAY(x) client::Float64Array *x = new client::Float64Array(1);
#define F64_ARRAY_SIZE(x) x->get_length()
#define F64_ARRAY_SETFROM(to,from) to = new client::Float64Array(from);
#define F64_ARRAY_CLEAR(x) x->fill(0);
#define F64_ARRAY_AT(x,i) (*x)[i]
#elif defined(USE_MAXVECTOR)
#define DECLARE_F64_ARRAY(x) maxi_vector x;
#define F64_ARRAY_SIZE(x) x.size()
#define F64_ARRAY_SETFROM(to,from) to.set(from);
#define F64_ARRAY_CLEAR(x) x.clear();
#define F64_ARRAY_AT(x,i) x[i]
#else
#define DECLARE_F64_ARRAY(x) std::vector<maxi_float_t> x;
#define F64_ARRAY_SIZE(x) x.size()
#define F64_ARRAY_SETFROM(to,from) to = from;
#define F64_ARRAY_CLEAR(x) x.clear();
#define F64_ARRAY_AT(x,i) x[i]

#endif

class CHEERP_EXPORT maxiSample
{

private:
    maxi_float_t position, recordPosition;
    maxi_float_t speed;
    maxi_float_t output;
    maxiLagExp<maxi_float_t> loopRecordLag;
    // DualModeF64Array test;

public:
//     //    int    myDataSize;
    short myChannels;
    int mySampleRate;
    inline long getLength() { return F64_ARRAY_SIZE(amplitudes); };
    // void setLength(unsigned long numSamples);
    short myBitsPerSample;
    maxiTrigger zxTrig;

    DECLARE_F64_ARRAY(amplitudes);

    // Constructor
    maxiSample();

#ifndef CHEERP
    maxiSample &operator=(const maxiSample &source)
    {
        if (this == &source)
            return *this;
        position = 0;
        recordPosition = 0;
        myChannels = source.myChannels;
        mySampleRate = maxiSettings::sampleRate;
        F64_ARRAY_SETFROM(amplitudes,source.amplitudes);
        return *this;
    }

    string myPath;
    int myChunkSize;
    int mySubChunk1Size;
    int readChannel;
    short myFormat;
    int myByteRate;
    short myBlockAlign;

    bool load(string fileName, int channel = 0);
    bool save();
    bool save(string filename);
    // read a wav file into this class
    bool read();

    // return a printable summary of the wav file
    string getSummary();

#endif
#ifdef VORBIS
    bool loadOgg(string filename, int channel = 0);
    int setSampleFromOggBlob(vector<unsigned char> &oggBlob, int channel = 0);
#endif
    // -------------------------
    bool isReady() {return F64_ARRAY_SIZE(amplitudes) > 1;}

    void setSample(maxi_number_tARRAY_REF _sampleData)
    {
        // NORMALISE_ARRAY_TYPE(_sampleData, sampleData)
        // amplitudes = sampleData;
        F64_ARRAY_SETFROM(amplitudes, _sampleData);
        // amplitudes.setFrom(_sampleData);
        mySampleRate = 44100;
        position = F64_ARRAY_SIZE(amplitudes) - 1;
    }

    void setSample(const maxi_number_tARRAY_REF _sampleData)
    {
        // NORMALISE_ARRAY_TYPE(_sampleData, sampleData)
        // amplitudes = sampleData;
        F64_ARRAY_SETFROM(amplitudes, _sampleData);
        // amplitudes.setFrom(_sampleData);
        mySampleRate = 44100;
        position = F64_ARRAY_SIZE(amplitudes) - 1;
    }

    void setSampleAndRate(maxi_number_tARRAY_REF _sampleData, int sampleRate)
    {
        setSample(_sampleData);
        mySampleRate = sampleRate;
    }

    void clear() { F64_ARRAY_CLEAR(amplitudes) }
    // // -------------------------

    void trigger();

    void loopRecord(maxi_float_t newSample, const bool recordEnabled, const maxi_float_t recordMix, maxi_float_t start, maxi_float_t end)
    {
        loopRecordLag.addSample(recordEnabled);
        if (recordPosition < start * F64_ARRAY_SIZE(amplitudes))
            recordPosition = start * F64_ARRAY_SIZE(amplitudes);
        if (recordEnabled)
        {
            maxi_float_t currentSample = F64_ARRAY_AT(amplitudes,(int)recordPosition) / 32767.0f;
            newSample = (recordMix * currentSample) + ((1.0f - recordMix) * newSample);
            newSample *= loopRecordLag.value();
            amplitudes[(unsigned long)recordPosition] = newSample * 32767;
        }
        ++recordPosition;
        if (recordPosition >= end * F64_ARRAY_SIZE(amplitudes))
            recordPosition = start * F64_ARRAY_SIZE(amplitudes);
    }

    void reset() {position=0;}

    maxi_float_t play();

    maxi_float_t playLoop(maxi_float_t start, maxi_float_t end); // start and end are between 0.0f and 1.0

    maxi_float_t playOnce();
    maxi_float_t playOnZX(maxi_float_t trigger);
    maxi_float_t playOnZXAtSpeed(maxi_float_t trig, maxi_float_t speed); //API CHANGE
    maxi_float_t playOnZXAtSpeedFromOffset(maxi_float_t trig, maxi_float_t speed, maxi_float_t offset); //API CHANGE
    maxi_float_t playOnZXAtSpeedBetweenPoints(maxi_float_t trig, maxi_float_t speed, maxi_float_t offset, maxi_float_t length); //API CHANGE

    maxi_float_t loopSetPosOnZX(maxi_float_t trigger, maxi_float_t position); // position between 0 and 1.0

    maxi_float_t playOnceAtSpeed(maxi_float_t speed); //API CHANGE

    void setPosition(maxi_float_t newPos); // between 0.0f and 1.0

    maxi_float_t playUntil(maxi_float_t end);
    maxi_float_t playUntilAtSpeed(maxi_float_t end, maxi_float_t speed);

    maxi_float_t playAtSpeed(maxi_float_t speed); //API CHANGE

    maxi_float_t playAtSpeedBetweenPointsFromPos(maxi_float_t frequency, maxi_float_t start, maxi_float_t end, maxi_float_t pos); //API CHANGE

    maxi_float_t playAtSpeedBetweenPoints(maxi_float_t frequency, maxi_float_t start, maxi_float_t end); //API CHANGE

    maxi_float_t play4(maxi_float_t frequency, maxi_float_t start, maxi_float_t end);


    void normalise(maxi_float_t maxLevel);                                                               //0 < maxLevel < 1.0
    void autoTrim(float alpha, float threshold, bool trimStart, bool trimEnd); //alpha of lag filter (lower == slower reaction), threshold to mark start and end, < 32767
};

class CHEERP_EXPORT maxiMap
{
public:
    maxiMap();
    static maxi_float_t inline linlin(maxi_float_t val, maxi_float_t inMin, maxi_float_t inMax, maxi_float_t outMin, maxi_float_t outMax)
    {
        val = max(min(val, inMax), inMin);
        return ((val - inMin) / (inMax - inMin) * (outMax - outMin)) + outMin;
    }

    static maxi_float_t inline linexp(maxi_float_t val, maxi_float_t inMin, maxi_float_t inMax, maxi_float_t outMin, maxi_float_t outMax)
    {
        //clipping
        val = max(min(val, inMax), inMin);
        return pow((outMax / outMin), (val - inMin) / (inMax - inMin)) * outMin;
    }

    static maxi_float_t inline explin(maxi_float_t val, maxi_float_t inMin, maxi_float_t inMax, maxi_float_t outMin, maxi_float_t outMax)
    {
        //clipping
        val = max(min(val, inMax), inMin);
        return (log(val / inMin) / log(inMax / inMin) * (outMax - outMin)) + outMin;
    }

    //replacing the templated version
    static maxi_float_t inline clamp(maxi_float_t v, const maxi_float_t low, const maxi_float_t high)
    {
        if (v > high)
        {
            v = high;
        }
        else if (v < low)
        {
            v = low;
        }
        return v;
    }
};

class maxiDyn
{

public:
    //	maxi_float_t gate(maxi_float_t input, maxi_float_t threshold=0.9, long holdtime=1, maxi_float_t attack=1, maxi_float_t release=0.9995);
    //	maxi_float_t compressor(maxi_float_t input, maxi_float_t ratio, maxi_float_t threshold=0.9, maxi_float_t attack=1, maxi_float_t release=0.9995);
    maxi_float_t gate(maxi_float_t input, maxi_float_t threshold = 0.9, long holdtime = 1, maxi_float_t attack = 1, maxi_float_t release = 0.9995);
    maxi_float_t compressor(maxi_float_t input, maxi_float_t ratio, maxi_float_t threshold = 0.9, maxi_float_t attack = 1, maxi_float_t release = 0.9995);
    maxi_float_t compress(maxi_float_t input);

    maxi_float_t input;
    maxi_float_t ratio;
    maxi_float_t currentRatio;
    maxi_float_t threshold;
    maxi_float_t output;
    maxi_float_t attack;
    maxi_float_t release;
    maxi_float_t amplitude;

    void setAttack(maxi_float_t attackMS);
    void setRelease(maxi_float_t releaseMS);
    void setThreshold(maxi_float_t thresholdI);
    void setRatio(maxi_float_t ratioF);
    long holdtime;
    long holdcount;
    int attackphase, holdphase, releasephase;

    // ------------------------------------------------
    // getters/setters
    //	int getTrigger() const{
    //		return trigger;
    //	}

    //	void setTrigger(int trigger){
    //		this->trigger = trigger;
    //	}

    // ------------------------------------------------
};

class maxiEnv
{

public:
    maxi_float_t ar(maxi_float_t input, maxi_float_t attack = 1, maxi_float_t release = 0.9, long holdtime = 1, int trigger = 0);
    maxi_float_t adsr(maxi_float_t input, maxi_float_t attack = 1, maxi_float_t decay = 0.99, maxi_float_t sustain = 0.125, maxi_float_t release = 0.9, long holdtime = 1, int trigger = 0);
    maxi_float_t adsr(maxi_float_t input, int trigger);
    maxi_float_t input;
    maxi_float_t output;
    maxi_float_t attack;
    maxi_float_t decay;
    maxi_float_t sustain;
    maxi_float_t release;
    maxi_float_t amplitude;

    void setAttack(maxi_float_t attackMS);
    void setRelease(maxi_float_t releaseMS);
    void setDecay(maxi_float_t decayMS);
    void setSustain(maxi_float_t sustainL);
    int trigger;

    long holdtime = 1;
    long holdcount;
    int attackphase, decayphase, sustainphase, holdphase, releasephase;

    // ------------------------------------------------
    // getters/setters
    int getTrigger() const
    {
        return trigger;
    }

    void setTrigger(int trigger)
    {
        this->trigger = trigger;
    }

    // ------------------------------------------------
};

class convert
{
public:
    static maxi_float_t mtof(int midinote);

    static maxi_float_t msToSamps(maxi_float_t timeMs)
    {
        return timeMs / 1000.0f * maxiSettings::sampleRate;
    }
};

class maxiSampleAndHold
{
public:
    inline maxi_float_t sah(maxi_float_t sigIn, maxi_float_t holdTimeMs)
    {
        maxi_float_t holdTimeSamples = convert::msToSamps(holdTimeMs);

        if (phase >= holdTimeSamples)
        {
            phase -= holdTimeSamples;
        }
        if (phase < 1.0f)
            holdValue = sigIn;
        phase++;
        return holdValue;
    }

private:
    maxi_float_t phase = 0;
    maxi_float_t holdValue = 0;
    bool firstRun = 1;
};

class maxiZeroCrossingDetector
{
public:
    maxiZeroCrossingDetector();
    inline bool zx(maxi_float_t x)
    {
        bool res = 0;
        if (previous_x <= 0 && x > 0)
        {
            res = 1;
        }
        previous_x = x;
        return res;
    }

private:
    maxi_float_t previous_x = 0;
};

//needs oversampling
class CHEERP_EXPORT maxiNonlinearity
{
public:
    maxiNonlinearity();
    /*atan distortion, see http://www.musicdsp.org/showArchiveComment.php?ArchiveID=104*/
    maxi_float_t atanDist(const maxi_float_t in, const maxi_float_t shape);
    /*shape from 1 (soft clipping) to infinity (hard clipping)*/
    maxi_float_t fastAtanDist(const maxi_float_t in, const maxi_float_t shape);
    maxi_float_t softclip(maxi_float_t x);
    maxi_float_t hardclip(maxi_float_t x);
    //asymmetric clipping: chose the shape of curves for both positive and negative values of x
    //try it here https://www.desmos.com/calculator/to6eixatsa
    maxi_float_t asymclip(maxi_float_t x, maxi_float_t a, maxi_float_t b);
    maxi_float_t fastatan(maxi_float_t x);
};

inline maxi_float_t maxiNonlinearity::asymclip(maxi_float_t x, maxi_float_t a, maxi_float_t b)
{

    if (x >= 1)
    {
        x = 1;
    }
    else if (x <= -1)
    {
        x = -1;
    }
    else if (x < 0)
    {
        x = -(pow(-x, a));
    }
    else
    {
        x = pow(x, b);
    }
    return x;
}

inline maxi_float_t maxiNonlinearity::hardclip(maxi_float_t x)
{
    x = x >= 1 ? 1 : (x <= -1 ? -1 : x);
    return x;
}
inline maxi_float_t maxiNonlinearity::softclip(maxi_float_t x)
{
    if (x >= 1)
    {
        x = 1;
    }
    else if (x <= -1)
    {
        x = -1;
    }
    else
    {
        x = (2.0f / 3.0f) * (x - powf(x, 3) / 3.0f);
    }
    return x;
}

inline maxi_float_t maxiNonlinearity::fastatan(maxi_float_t x)
{
    return (x / (1.0f + 0.28f * (x * x)));
}

inline maxi_float_t maxiNonlinearity::atanDist(const maxi_float_t in, const maxi_float_t shape)
{
    maxi_float_t out;
    out = (1.0f / atan(shape)) * atan(in * shape);
    return out;
}

inline maxi_float_t maxiNonlinearity::fastAtanDist(const maxi_float_t in, const maxi_float_t shape)
{
    maxi_float_t out;
    out = (1.0f / fastatan(shape)) * fastatan(in * shape);
    return out;
}

using maxiDistortion = maxiNonlinearity; // backwards compatibility

class maxiFlanger
{
public:
    //delay = delay time - ~800 sounds good
    //feedback = 0 - 1
    //speed = lfo speed in Hz, 0.0001 - 10 sounds good
    //depth = 0 - 1
    maxi_float_t flange(const maxi_float_t input, const unsigned int delay, const maxi_float_t feedback, const maxi_float_t speed, const maxi_float_t depth);
    maxiDelayline dl;
    maxiOsc lfo;
};

inline maxi_float_t maxiFlanger::flange(const maxi_float_t input, const unsigned int delay, const maxi_float_t feedback, const maxi_float_t speed, const maxi_float_t depth)
{
    //todo: needs fixing
    maxi_float_t output;
    maxi_float_t lfoVal = lfo.triangle(speed);
    output = dl.dl(input, delay + (lfoVal * depth * delay) + 1, feedback);
    maxi_float_t normalise = (1 - fabs(output));
    output *= normalise;
    return (output + input) / 2.0f;
}

class maxiChorus
{
public:
    //delay = delay time - ~800 sounds good
    //feedback = 0 - 1
    //speed = lfo speed in Hz, 0.0001 - 10 sounds good
    //depth = 0 - 1
    maxi_float_t chorus(const maxi_float_t input, const unsigned int delay, const maxi_float_t feedback, const maxi_float_t speed, const maxi_float_t depth);
    maxiDelayline dl, dl2;
    maxiOsc lfo;
    maxiFilter lopass;
};

inline maxi_float_t maxiChorus::chorus(const maxi_float_t input, const unsigned int delay, const maxi_float_t feedback, const maxi_float_t speed, const maxi_float_t depth)
{
    //this needs fixing
    maxi_float_t output1, output2;
    maxi_float_t lfoVal = lfo.noise();
    lfoVal = lopass.lores(lfoVal, speed, 1.0f) * 2.0f;
    output1 = dl.dl(input, delay + (lfoVal * depth * delay) + 1, feedback);
    output2 = dl2.dl(input, (delay + (lfoVal * depth * delay * 1.02f) + 1) * 0.98f, feedback * 0.99f);
    output1 *= (1.0f - fabs(output1));
    output2 *= (1.0f - fabs(output2));
    return (output1 + output2 + input) / 3.0f;
}

template <typename T>
class maxiEnvelopeFollowerType
{
public:
    maxiEnvelopeFollowerType()
    {
        setAttack(100);
        setRelease(100);
        env = 0;
    }
    void setAttack(T attackMS)
    {
        attack = pow(0.01f, 1.0f / (attackMS * maxiSettings::sampleRate * 0.001));
    }
    void setRelease(T releaseMS)
    {
        release = pow(0.01f, 1.0f / (releaseMS * maxiSettings::sampleRate * 0.001));
    }
    inline T play(T input)
    {
        input = fabs(input);
        if (input > env)
            env = attack * (env - input) + input;
        else
            env = release * (env - input) + input;
        return env;
    }
    void reset() { env = 0; }
    inline T getEnv() { return env; }
    inline void setEnv(T val) { env = val; }

private:
    T attack, release, env;
};

typedef maxiEnvelopeFollowerType<maxi_float_t> maxiEnvelopeFollower;
typedef maxiEnvelopeFollowerType<float> maxiEnvelopeFollowerF;

class maxiDCBlocker
{
public:
    maxi_float_t xm1, ym1;
    maxiDCBlocker() : xm1(0), ym1(0) {}
    inline maxi_float_t play(maxi_float_t input, maxi_float_t R)
    {
        ym1 = input - xm1 + R * ym1;
        xm1 = input;
        return ym1;
    }
};

/*
 State Variable Filter

 algorithm from  http://www.cytomic.com/files/dsp/SvfLinearTrapOptimised.pdf
 usage:
 either set the parameters separately as required (to save CPU)

 filter.setCutoff(param1);
 filter.setResonance(param2);

 w = filter.play(w, 0.0, 1.0, 0.0, 0.0f);

 or set everything together at once

 w = filter.setCutoff(param1).setResonance(param2).play(w, 0.0, 1.0, 0.0, 0.0f);

 */
class maxiSVF
{
public:
    maxiSVF() : v0z(0), v1(0), v2(0) { setParams(1000, 1); }

    //20 < cutoff < 20000
    inline void setCutoff(maxi_float_t cutoff)
    {
        setParams(cutoff, res);
    }

    //from 0 upwards, starts to ring from 2-3ish, cracks a bit around 10
    inline void setResonance(maxi_float_t q)
    {
        setParams(freq, q);
    }

    //run the filter, and get a mixture of lowpass, bandpass, highpass and notch outputs
    inline maxi_float_t play(maxi_float_t w, maxi_float_t lpmix, maxi_float_t bpmix, maxi_float_t hpmix, maxi_float_t notchmix)
    {
        maxi_float_t low, band, high, notch;
        maxi_float_t v1z = v1;
        maxi_float_t v2z = v2;
        maxi_float_t v3 = w + v0z - 2.0f * v2z;
        v1 += g1 * v3 - g2 * v1z;
        v2 += g3 * v3 + g4 * v1z;
        v0z = w;
        low = v2;
        band = v1;
        high = w - k * v1 - v2;
        notch = w - k * v1;
        return (low * lpmix) + (band * bpmix) + (high * hpmix) + (notch * notchmix);
    }

private:
    inline void setParams(maxi_float_t _freq, maxi_float_t _res)
    {
        freq = _freq;
        res = _res;
        g = tan(PI * freq / maxiSettings::sampleRate);
        damping = res == 0 ? 0 : 1.0f / res;
        k = damping;
        ginv = g / (1.0f + g * (g + k));
        g1 = ginv;
        g2 = 2.0f * (g + k) * ginv;
        g3 = g * ginv;
        g4 = 2.0f * ginv;
    }

    maxi_float_t v0z, v1, v2, g, damping, k, ginv, g1, g2, g3, g4;
    maxi_float_t freq, res;
};

//based on http://www.earlevel.com/main/2011/01/02/biquad-formulas/ and https://ccrma.stanford.edu/~jos/fp/Direct_Form_II.html
class CHEERP_EXPORT maxiBiquad
{
public:
    maxiBiquad();
    enum filterTypes
    {
        LOWPASS,
        HIGHPASS,
        BANDPASS,
        NOTCH,
        PEAK,
        LOWSHELF,
        HIGHSHELF
    };
    inline maxi_float_t play(maxi_float_t input)
    {
        v[0] = input - (b1 * v[1]) - (b2 * v[2]);
        maxi_float_t y = (a0 * v[0]) + (a1 * v[1]) + (a2 * v[2]);
        v[2] = v[1];
        v[1] = v[0];
        return y;
    }
    inline void set(filterTypes filtType, maxi_float_t cutoff, maxi_float_t Q, maxi_float_t peakGain)
    {
        maxi_float_t norm = 0;
        maxi_float_t V = pow(10.0f, abs(peakGain) / 20.0f);
        maxi_float_t K = tan(PI * cutoff / maxiSettings::sampleRate);
        switch (filtType)
        {
        case LOWPASS:
            norm = 1.0f / (1.0f + K / Q + K * K);
            a0 = K * K * norm;
            a1 = 2.0f * a0;
            a2 = a0;
            b1 = 2.0f * (K * K - 1.0f) * norm;
            b2 = (1.0f - K / Q + K * K) * norm;
            break;

        case HIGHPASS:
            norm = 1.0f / (1.0f + K / Q + K * K);
            a0 = 1 * norm;
            a1 = -2 * a0;
            a2 = a0;
            b1 = 2 * (K * K - 1) * norm;
            b2 = (1 - K / Q + K * K) * norm;
            break;

        case BANDPASS:
            norm = 1.0f / (1.0f + K / Q + K * K);
            a0 = K / Q * norm;
            a1 = 0.;
            a2 = -a0;
            b1 = 2.f * (K * K - 1.f) * norm;
            b2 = (1.0f - K / Q + K * K) * norm;
            break;

        case NOTCH:
            norm = 1.0f / (1.0f + K / Q + K * K);
            a0 = (1.0f + K * K) * norm;
            a1 = 2.f * (K * K - 1.f) * norm;
            a2 = a0;
            b1 = a1;
            b2 = (1.0f - K / Q + K * K) * norm;
            break;

        case PEAK:
            if (peakGain >= 0.0f)
            { // boost
                norm = 1.0f / (1.0f + 1.0f / Q * K + K * K);
                a0 = (1.0f + V / Q * K + K * K) * norm;
                a1 = 2.f * (K * K - 1.f) * norm;
                a2 = (1.0f - V / Q * K + K * K) * norm;
                b1 = a1;
                b2 = (1.0f - 1.0f / Q * K + K * K) * norm;
            }
            else
            { // cut
                norm = 1.0f / (1.0f + V / Q * K + K * K);
                a0 = (1.0f + 1 / Q * K + K * K) * norm;
                a1 = 2.f * (K * K - 1) * norm;
                a2 = (1.0f - 1.0f / Q * K + K * K) * norm;
                b1 = a1;
                b2 = (1.0f - V / Q * K + K * K) * norm;
            }
            break;
        case LOWSHELF:
            if (peakGain >= 0.f)
            { // boost
                norm = 1.0f / (1.0f + SQRT2 * K + K * K);
                a0 = (1.0f + sqrt(2.f * V) * K + V * K * K) * norm;
                a1 = 2.f * (V * K * K - 1.f) * norm;
                a2 = (1.0f - sqrt(2.f * V) * K + V * K * K) * norm;
                b1 = 2.f * (K * K - 1.f) * norm;
                b2 = (1.0f - SQRT2 * K + K * K) * norm;
            }
            else
            { // cut
                norm = 1.0f / (1.0f + sqrt(2.f * V) * K + V * K * K);
                a0 = (1.0f + SQRT2 * K + K * K) * norm;
                a1 = 2.f * (K * K - 1.f) * norm;
                a2 = (1.0f - SQRT2 * K + K * K) * norm;
                b1 = 2.f * (V * K * K - 1.f) * norm;
                b2 = (1.0f - sqrt(2.f * V) * K + V * K * K) * norm;
            }
            break;
        case HIGHSHELF:
            if (peakGain >= 0.f)
            { // boost
                norm = 1.0f / (1.0f + SQRT2 * K + K * K);
                a0 = (V + sqrt(2.f * V) * K + K * K) * norm;
                a1 = 2.f * (K * K - V) * norm;
                a2 = (V - sqrt(2.f * V) * K + K * K) * norm;
                b1 = 2.f * (K * K - 1) * norm;
                b2 = (1.0f - SQRT2 * K + K * K) * norm;
            }
            else
            { // cut
                norm = 1.0f / (V + sqrt(2.f * V) * K + K * K);
                a0 = (1.0f + SQRT2 * K + K * K) * norm;
                a1 = 2.f * (K * K - 1.f) * norm;
                a2 = (1.0f - SQRT2 * K + K * K) * norm;
                b1 = 2.f * (K * K - V) * norm;
                b2 = (V - sqrt(2.f * V) * K + K * K) * norm;
            }
            break;
        }
    }

private:
    maxi_float_t a0 = 0, a1 = 0, a2 = 0, b1 = 0, b2 = 0;
    filterTypes filterType;
    const maxi_float_t SQRT2 = sqrt(2.0f);
    maxi_float_t v[3] = {0, 0, 0};
};

class maxiXFade
{
public:
    maxiXFade() {}
    static vector<maxi_float_t> xfade(vector<maxi_float_t> &ch1, vector<maxi_float_t> &ch2, maxi_float_t xfader)
    {
        xfader = maxiMap::clamp(xfader, -1, 1);
        maxi_float_t xfNorm = maxiMap::linlin(xfader, -1, 1, 0, 1);
        maxi_float_t gainCh1 = sqrt(1.0f - xfNorm);
        maxi_float_t gainCh2 = sqrt(xfNorm);
        vector<maxi_float_t> output(ch1.size(), 0.0f);
        for (size_t i = 0; i < output.size(); i++)
        {
            output[i] = (ch1[i] * gainCh1) + (ch2[i] * gainCh2);
        }
        return output;
    }
    static maxi_float_t xfade(maxi_float_t ch1, maxi_float_t ch2, maxi_float_t xfader)
    {
        vector<maxi_float_t> vch1 = {ch1};
        vector<maxi_float_t> vch2 = {ch2};
        return maxiXFade::xfade(vch1, vch2, xfader)[0];
    }
};

class maxiLine
{
public:
    maxiLine() {}
    inline maxi_float_t play(maxi_float_t trigger)
    {
        if (!lineComplete)
        {
            if (trigEnable && !triggered)
            {
                triggered = (trigger > 0.0f && lastTrigVal <= 0.0f);
                lineValue = lineStart;
            }
            if (triggered)
            {
                lineValue += inc;
                if (inc <= 0)
                {
                    lineComplete = lineValue <= lineEnd;
                }
                else
                {
                    lineComplete = lineValue >= lineEnd;
                }
                if (lineComplete)
                {
                    if (!oneShot)
                    {
                        reset();
                    }
                }
            }
            lastTrigVal = trigger;
        }
        return lineValue;
    }
    inline void prepare(maxi_float_t start, maxi_float_t end, maxi_float_t durationMs, bool isOneShot)
    {
        lineValue = lineStart;
        lineStart = start;
        lineEnd = end;
        maxi_float_t lineMag = end - start;
        maxi_float_t durInSamples = durationMs / 1000.0f * maxiSettings::sampleRate;
        inc = lineMag / durInSamples;
        oneShot = isOneShot;
        reset();
    }
    inline void triggerEnable(maxi_float_t on)
    {
        trigEnable = on > 0.0f;
    }
    inline bool isLineComplete()
    {
        return lineComplete;
    }

private:
    maxi_float_t phase = 0;
    maxi_float_t lineValue = 0;
    maxi_float_t inc = 0;
    maxi_float_t lastTrigVal = -1;
    maxi_float_t trigEnable = false;
    maxi_float_t triggered = false;
    bool lineComplete = false;
    maxi_float_t lineStart = 0;
    maxi_float_t lineEnd = 0;
    bool oneShot = 1;
    void reset()
    {
        triggered = false;
        lineComplete = false;
    }
};

class maxiMath
{
public:
    static maxi_float_t add(maxi_float_t x, maxi_float_t y)
    {
        return x + y;
    };
    static maxi_float_t div(maxi_float_t x, maxi_float_t y)
    {
        return x / y;
    };
    static maxi_float_t mul(maxi_float_t x, maxi_float_t y)
    {
        return x * y;
    };
    static maxi_float_t sub(maxi_float_t x, maxi_float_t y)
    {
        return x - y;
    };
    static maxi_float_t gt(maxi_float_t x, maxi_float_t y)
    {
        return x > y;
    }
    static maxi_float_t lt(maxi_float_t x, maxi_float_t y)
    {
        return x < y;
    }
    static maxi_float_t gte(maxi_float_t x, maxi_float_t y)
    {
        return x >= y;
    }
    static maxi_float_t lte(maxi_float_t x, maxi_float_t y)
    {
        return x <= y;
    }
    static maxi_float_t mod(maxi_float_t x, maxi_float_t y)
    {
        return fmod(x, y);
    }
    static maxi_float_t abs(maxi_float_t x)
    {
        return fabs(x);
    }
    static maxi_float_t xpowy(maxi_float_t x, maxi_float_t y)
    {
        return pow(x, y);
    }
};

//https://tutorials.siam.org/dsweb/cotutorial/index.php?s=3&p=0
//https://www.complexity-explorables.org/explorables/ride-my-kuramotocycle/
class maxiKuramotoOscillator
{
public:
    maxiKuramotoOscillator() {}
    inline maxi_float_t play(maxi_float_t freq, maxi_float_t K, std::vector<maxi_float_t> phases)
    {

        maxi_float_t phaseAdj = 0;
        for (maxi_float_t v : phases)
        {
            phaseAdj += sin(v - phase);
        }
        phase += dt * (freq + ((K / phases.size()) * phaseAdj));
        if (phase >= TWOPI)
            phase -= TWOPI;
        else if (phase < 0)
            phase += TWOPI;
        return phase;
    }
    inline void setPhase(maxi_float_t newPhase) { phase = newPhase; }
    inline maxi_float_t getPhase() { return phase; }

private:
    maxi_float_t phase = 0.0f;
    maxi_float_t dt = TWOPI / maxiSettings::sampleRate;
};

//a local group of oscillators
class maxiKuramotoOscillatorSet
{
public:
    maxiKuramotoOscillatorSet(const size_t N)
    {
        oscs.resize(N);
        phases.resize(N);
    };
    void setPhases(const std::vector<maxi_float_t> &phases)
    {
        size_t iOsc = 0;
        for (maxi_float_t v : phases)
        {
            oscs[iOsc].setPhase(v);
            iOsc++;
        }
    }

    void setPhase(const maxi_float_t phase, const size_t oscillatorIdx)
    {
        oscs[oscillatorIdx].setPhase(phase);
    }

    maxi_float_t getPhase(size_t i)
    {
        return oscs[i].getPhase();
    }

    size_t size()
    {
        return oscs.size();
    }

    maxi_float_t play(maxi_float_t freq, maxi_float_t K)
    {
        maxi_float_t mix = 0.0f;
        //gather phases
        for (size_t i = 0; i < phases.size(); i++)
        {
            phases[i] = oscs[i].getPhase();
        }
        for (auto &v : oscs)
        {
            mix += v.play(freq, K, phases);
        }
        return mix / phases.size();
    }

protected:
    std::vector<maxiKuramotoOscillator> oscs;
    std::vector<maxi_float_t> phases;
};

//a single oscillator, updated according to phase information from remote oscillators
//best guesses of the remote oscillators are maintained, and asynchronously updated
//use case: a networked clock
class maxiAsyncKuramotoOscillator : public maxiKuramotoOscillatorSet
{
public:
    //1 local oscillator and N-1 remote oscillators
    maxiAsyncKuramotoOscillator(const size_t N) : maxiKuramotoOscillatorSet(N){};

    void setPhase(const maxi_float_t phase, const size_t oscillatorIdx)
    {
        oscs[oscillatorIdx].setPhase(phase);
        update = 1;
    }
    void setPhases(const std::vector<maxi_float_t> &phases)
    {
        size_t iOsc = 0;
        for (maxi_float_t v : phases)
        {
            oscs[iOsc].setPhase(v);
            iOsc++;
        }
        update = 1;
    }

    maxi_float_t play(maxi_float_t freq, maxi_float_t K)
    {
        maxi_float_t mix = 0.0f;
        //gather phases
        if (update)
        {
            for (size_t i = 0; i < phases.size(); i++)
            {
                phases[i] = oscs[i].getPhase();
            }
        }
        for (auto &v : oscs)
        {
            mix += v.play(freq, update ? K : 0, phases);
        }
        update = 0;
        return mix / phases.size();
    }

    maxi_float_t getPhase(size_t i)
    {
        return maxiKuramotoOscillatorSet::getPhase(i);
    }

    size_t size()
    {
        return maxiKuramotoOscillatorSet::size();
    }

private:
    bool update = 0;
};

class maxiBits
{
public:
    typedef uint32_t bitsig;

    // static bitsig sig(bitsig v) return v;
    // maxiBits() {}
    // maxiBits(const bitsig v) : t(v) {}

    static bitsig sig(bitsig v) { return v; }

    static bitsig at(const bitsig v, const bitsig idx)
    {
        return 1 & (v >> idx);
    }
    static bitsig shl(const bitsig v, const bitsig shift)
    {
        return v << shift;
    }
    static bitsig shr(const bitsig v, const bitsig shift)
    {
        return v >> shift;
    }
    static bitsig r(const bitsig v, const bitsig offset, const bitsig width)
    {
        bitsig mask = maxiBits::l(width);
        bitsig shift = offset - width + 1;
        bitsig x = 0;
        x = v & shl(mask, shift);
        x = x >> shift;
        return x;
    }
    static bitsig land(const bitsig v, const bitsig x)
    {
        return v & x;
    }
    static bitsig lor(const bitsig v, const bitsig x)
    {
        return v | x;
    }
    static bitsig lxor(const bitsig v, const bitsig x)
    {
        return v ^ x;
    }
    static bitsig neg(const bitsig v)
    {
        return ~v;
    }
    static bitsig inc(const bitsig v)
    {
        return v + 1;
    }
    static bitsig dec(const bitsig v)
    {
        return v - 1;
    }
    static bitsig add(const bitsig v, const bitsig m)
    {
        return v + m;
    }
    static bitsig sub(const bitsig v, const bitsig m)
    {
        return v - m;
    }
    static bitsig mul(const bitsig v, const bitsig m)
    {
        return v * m;
    }
    static bitsig div(const bitsig v, const bitsig m)
    {
        return v / m;
    }
    static bitsig gt(const bitsig v, const bitsig m)
    {
        return v > m;
    }
    static bitsig lt(const bitsig v, const bitsig m)
    {
        return v < m;
    }
    static bitsig gte(const bitsig v, const bitsig m)
    {
        return v >= m;
    }
    static bitsig lte(const bitsig v, const bitsig m)
    {
        return v <= m;
    }
    static bitsig eq(const bitsig v, const bitsig m)
    {
        return v == m;
    }
    static bitsig ct(const bitsig v, const bitsig width)
    {
        bitsig x = 0;
        for (size_t i = 0; i < width; i++)
        {
            x += (v & (1 << i)) > 0;
        }
        return x;
    }
    static bitsig l(const bitsig width)
    {
        bitsig v = 0;
        for (size_t i = 0; i < width; i++)
        {
            v += (1 << i);
        }
        return v;
    }

    static bitsig noise()
    {
        bitsig v = static_cast<bitsig>(rand());
        return v;
    }

    static maxi_float_t toSignal(const bitsig t)
    {
        return maxiMap::linlin(t, 0, (maxi_float_t)std::numeric_limits<uint32_t>::max(), -1, 1);
    }

    static maxi_float_t toTrigSignal(const bitsig t)
    {
        return t > 0 ? 1.0f : -1.0;
    }

    static bitsig fromSignal(const maxi_float_t t)
    {
        const bitsig halfRange = (std::numeric_limits<uint32_t>::max() / 2);
        const bitsig val = halfRange + (t * (halfRange - 1));
        return val;
    }

    // void sett(maxiBits::bitsig v){t=v;}
    // maxiBits::bitsig gett() const {return t;};

    // maxiBits::bitsig t=0;
};

class maxiCounter
{
public:
    maxi_float_t count(maxi_float_t incTrigger, maxi_float_t resetTrigger)
    {
        if (inctrig.onZX(incTrigger))
        {
            value++;
        }
        if (rstrig.onZX(resetTrigger))
        {
            value = 0;
        }
        return value;
    }

private:
    maxi_float_t value = 0;
    maxiTrigger inctrig, rstrig;
};

class CHEERP_EXPORT maxiIndex
{
public:
    maxiIndex();
    maxi_float_t pull(const maxi_float_t trigSig, maxi_float_t indexSig, maxi_number_tARRAY_REF _values)
    {
        // maxi_float_t *__arrayStart = __builtin_cheerp_make_regular<maxi_float_t>(_values, 0);
        // size_t __arrayLength = _values->get_length();
        // vector<maxi_float_t> values = vector<maxi_float_t>(__arrayStart, __arrayStart + __arrayLength);
        NORMALISE_ARRAY_TYPE(_values, values)
        if (trig.onZX(trigSig))
        {
            if (indexSig < 0)
                indexSig = 0;
            if (indexSig > 1)
                indexSig = 1;
            size_t arrayIndex = static_cast<size_t>(floor(indexSig * 0.99999999f * values.size()));
            value = values[arrayIndex];
        }
        return value;
    }

private:
    maxiTrigger trig;
    maxi_float_t value = 0;
};

class CHEERP_EXPORT maxiRatioSeq
{
public:
    maxiRatioSeq();
    maxi_float_t playTrig(maxi_float_t phase, maxi_number_tARRAY_REF times)
    {
        // NORMALISE_ARRAY_TYPE(_times, times)
        maxi_float_t trig = 0;
        // maxi_float_t sum = std::accumulate(times.begin(), times.end(), 0);
        maxi_float_t sum=0;
        size_t seqlen = F64_ARRAY_SIZE(times);
        for(size_t i=0; i < seqlen; i++) sum += F64_ARRAY_AT(times,i);
        maxi_float_t accumulatedTime = 0;
        for (size_t i = 0; i < seqlen; i++)
        {
            accumulatedTime += F64_ARRAY_AT(times,i);
            maxi_float_t normalisedTime = accumulatedTime / sum;
            if (normalisedTime == 1.0f  )
                normalisedTime = 0.0f;
            if (prevPhase > phase)
            {
                //wrapping point
                prevPhase = -1.0f / maxiSettings::sampleRate;
            }
            if ((prevPhase <= normalisedTime && phase > normalisedTime))
            {
                trig = 1;
                break;
            }
        }
        prevPhase = phase;
        return trig;
    }

    maxi_float_t playValues(maxi_float_t phase, maxi_number_tARRAY_REF times, maxi_number_tARRAY_REF values)
    {
        // NORMALISE_ARRAY_TYPE(_times, times)
        // NORMALISE_ARRAY_TYPE(_values, values)
        size_t vallen = F64_ARRAY_SIZE(values);
        if (lengthOfValues != vallen)
        {
            lengthOfValues = vallen;
            counter = lengthOfValues - 1;
        }
        if (playTrig(phase, times))
        {
            counter++;
            if (counter == vallen)
            {
                counter = 0;
            }
        }
        return F64_ARRAY_AT(values,counter);
    }

private:
    maxi_float_t prevPhase = 0;
    size_t counter = 0;
    size_t lengthOfValues = 0;
};

class CHEERP_EXPORT maxiZXToPulse
{
public:
    maxiZXToPulse();
    double play(double input, double holdTimeInSamples) {
        double output =0;
        
        if (trig.onZX(input)) {
            holdCounter = holdTimeInSamples;
        }

        if (holdCounter > 0) {
            output = 1;
            holdCounter--;
        }

        return output;
    }
private:
    maxiTrigger trig;
    double holdCounter = 0;
};

#endif
