

#ifndef _MAXI_GRAINS_H
#define _MAXI_GRAINS_H
#include "maxiMalloc.h"
#include "../maximilian.h"
#if defined(__APPLE_CC__) && !defined(ARDUINO)
#include "accelerate/accelerate.h"
//Mac users can uncommment the line below to use Apple's accelerate framework for calculating grains. This gives ~300% speed improvement and better sound quality, but doesn't work well on all machines.
//#define MAXIGRAINFAST
#endif

#include <list>

typedef unsigned long ulong;

//window functions from http://en.wikipedia.org/wiki/Window_function#High-_and_moderate-resolution_windows

struct hannWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return 0.5f * (1.0f - cos((2.0f * PI * windowPos) / (windowLength - 1)));
	}
};

//this window can produce clicks
struct hammingWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return 0.54f - (0.46f * cos((2.0f * PI * windowPos) / (windowLength - 1)));
	}
};


struct cosineWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return sin((PI * windowPos) / (windowLength - 1));
	}
};

struct rectWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return 1;
	}
};

struct triangleWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return (2.0f / (windowLength-1.0f)) * (((windowLength-1.0f)/2.0f) - fabs(windowPos - ((windowLength-1.0f)/2.0f)));
	}
};

struct triangleNZWinFunctor {
	//non zero end points
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return (2.0f / windowLength) * ((windowLength/2.0f) - fabs(windowPos - ((windowLength-1.0f)/2.0f)));
	}
};

struct blackmanHarrisWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return 0.35875f -
		(0.48829f * cos((2 * PI * windowPos) / (windowLength-1))) +
		(0.14128f * cos((4 * PI * windowPos) / (windowLength-1))) +
		(0.01168f * cos((6 * PI * windowPos) / (windowLength-1)));
	}
};

struct blackmanNutallWinFunctor {
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
		return 0.3635819f -
		(0.4891775f * cos((2 * PI * windowPos) / (windowLength-1))) +
		(0.1365995f * cos((4 * PI * windowPos) / (windowLength-1))) +
		(0.0106411f * cos((6 * PI * windowPos) / (windowLength-1)));
	}
};

struct gaussianWinFunctor {
    maxi_float_t gausDivisor;
    gaussianWinFunctor() {
        init(0.3);
    }
    gaussianWinFunctor(maxi_float_t kurtosis) {
        init(kurtosis);
    }
    void init(maxi_float_t kurtosis) {
        gausDivisor = (-2.0f * kurtosis * kurtosis);
    }
	inline maxi_float_t operator()(ulong windowLength, ulong windowPos) {
        maxi_float_t phase = ((windowPos / (maxi_float_t) windowLength) - 0.5f) * 2.0f;
        return exp((phase * phase) / gausDivisor);
	}
};

// Renamend T to TFunc because of compile errors on ESP32
template<typename TFunc>
class maxiGrainWindowCache {
public:
	unsigned int cacheSize;

	maxiGrainWindowCache() {
		cacheSize = maxiSettings::sampleRate / 2.0f; //allocate mem for up to 500ms grains
		// esp32 avoid allocation in constructor because psram is not available yet!
	}

	~maxiGrainWindowCache() {
		if (cache == nullptr) return;
		for(int i=0; i < cacheSize; i++) {
			if(NULL != cache[i]) {
				free(cache[i]);
			}
		}
        free(cache);
	}

	maxi_float_t* getWindow(const unsigned int length) {
		alloc();
		if (NULL == cache[length]) {
			cache[length] = (maxi_float_t*) maxi_malloc(length * sizeof(maxi_float_t));
			for(int i=0; i < length; i++) {
				cache[length][i] = TFunc()(length, i);
			}
		}
		return cache[length];
	}

private:
	maxi_float_t** cache = nullptr;

	void alloc() {
		if (cache == nullptr) {
			cache = (maxi_float_t**) maxi_malloc(cacheSize * sizeof(maxi_float_t*));
			for(int i=0; i < cacheSize; i++) {
				cache[i] = NULL;
			}
		}
	}

};

class maxiGrainBase {
public:
    virtual maxi_float_t play()=0;
    virtual ~maxiGrainBase() {}
	bool finished;
};

template<typename F>
class maxiGrain : public maxiGrainBase {
public:
	maxiSample *sample;
    maxi_float_t pos;
	maxi_float_t dur;
	unsigned long sampleStartPos;
	unsigned long sampleIdx;
	unsigned long sampleDur;
	unsigned long sampleEndPos;
	maxi_float_t freq;
	maxi_float_t speed;
	maxi_float_t inc;
	maxi_float_t frequency;
	maxi_float_t* window;
    maxi_float_t* buffer;
#if defined(__APPLE_CC__) && defined(MAXIGRAINFAST)
	maxi_float_t* grainSamples;
#endif
	/*
	 position between 0.0f and 1.0
	 duration in seconds
	 */
	maxiGrain(maxiSample *sample, const maxi_float_t position, const maxi_float_t duration, const maxi_float_t speed, maxiGrainWindowCache<F> *windowCache) :sample(sample), pos(position), dur(duration), speed(speed)
	{
        buffer = &sample->amplitudes[0];
//        buffer = sample->temp;
		sampleStartPos = sample->getLength() * pos;
		sampleDur = dur * (maxi_float_t)sample->mySampleRate;
		sampleDurMinusOne = sampleDur - 1;
		sampleIdx = 0;
		finished = 0;
		freq = 1.0f / dur;
		sampleEndPos = min((long unsigned int)sample->getLength(), sampleStartPos + sampleDur);
		frequency = freq * speed;
		if (frequency > 0) {
			pos = sampleStartPos;
		}else{
			pos = sampleEndPos;
		}
        if (frequency != 0) {
            inc = sampleDur/(maxiSettings::sampleRate/frequency);
        }else
            inc = 0;
		window = windowCache->getWindow(sampleDur);

#if defined(__APPLE_CC__) && defined(MAXIGRAINFAST)
		//premake the grain using fast vector functions, and quadratic interpolation
		maxi_float_t *sourceData = (maxi_float_t*) maxi_malloc(sampleDur * sizeof(maxi_float_t));
		short* buffer = (short *)sample->temp.data();
		//convert sample to maxi_float_t data
		vDSP_vflt16D(buffer + sampleStartPos, 1, sourceData, 1, min(sampleDur, sample->length - sampleStartPos));
		//todo: wraping code

		grainSamples = (maxi_float_t*) maxi_malloc(sampleDur * sizeof(maxi_float_t));
		//make list of interpolation indexes
		maxi_float_t* interpIndexes = (maxi_float_t*) maxi_malloc(sampleDur * sizeof(maxi_float_t));
		maxi_float_t interpPos = sampleStartPos;
		for(int i=0; i < sampleDur; i++) {
			interpIndexes[i] = interpPos - sampleStartPos;
			interpPos += fabs(inc);
		}
		vDSP_vqintD(sourceData, interpIndexes, 1, grainSamples, 1, sampleDur, sampleDur);
		if (frequency < 0) {
			vDSP_vrvrsD(grainSamples,1, sampleDur);
		}
		static maxi_float_t divFactor = 32767.0;
		vDSP_vsdivD(grainSamples, 1, &divFactor, grainSamples, 1, sampleDur);
		vDSP_vmulD(grainSamples, 1, window, 1, grainSamples, 1, sampleDur);
		delete sourceData, interpIndexes;
#endif
	}

	~maxiGrain() {
#if defined(__APPLE_CC__) && defined(MAXIGRAINFAST)
		delete[] grainSamples;
#endif
	}

	inline maxi_float_t play() {
		maxi_float_t output = 0.0f;
		if (!finished) {
#if defined(__APPLE_CC__) && defined(MAXIGRAINFAST)
			output = grainSamples[sampleIdx];
#else
			envValue = window[sampleIdx];
			maxi_float_t remainder;
            pos += inc;
            if (pos >= sample->getLength())
                pos -= sample->getLength();
            else if (pos < 0)
                pos += sample->getLength();

            long posl = floor(pos);
            remainder = pos - posl;
            long a = posl;
            long b = posl+1;
            if (b >= sample->getLength()) {
                b = 0;
            }
            output = (maxi_float_t) ((1-remainder) * buffer[a] +
                               remainder * buffer[b]);//linear interpolation
			output *= envValue;
#endif
		}
		sampleIdx++;
		if (sampleIdx == sampleDur) finished = true;
		return output;
	}

protected:
	maxiGrain();
	maxi_float_t envValue;
	ulong sampleDurMinusOne;
};


typedef list<maxiGrainBase*> grainList;

class maxiGrainPlayer {
public:
	grainList grains;
	maxiSample *sample;

//	maxiGrainPlayer(){}

	maxiGrainPlayer(maxiSample *sample) : sample(sample) {
	}

	void addGrain(maxiGrainBase *g) {
		grains.push_back(g);
	}

	inline maxi_float_t play() {
		maxi_float_t total = 0.0f;
        grainList::iterator it = grains.begin();
		while(it != grains.end()) {
			total += (*it)->play();
			if ((*it)->finished) {
				delete(*it);
				it = grains.erase(it);
			}else{
                it++;
            }
		}
		return total;
	}
};

template<typename F>
class maxiTimeStretch {
protected:
    maxi_float_t position;
public:
	maxiSample *sample;
	maxiGrainPlayer *grainPlayer;
	maxiGrainWindowCache<F> windowCache;
	maxi_float_t randomOffset;
    maxi_float_t looper;

	maxiTimeStretch(){
		position=0;
		looper = 0;
		randomOffset=0;
	}

	maxiTimeStretch(maxiSample *sample) : sample(sample) {
		position=0;
        looper = 0;
		grainPlayer = new maxiGrainPlayer(sample);
		randomOffset=0;
	}

	~maxiTimeStretch() {
		delete grainPlayer;
	}

	void setSample(maxiSample* sampleIn){
		// could just erase the sample in player
		// and replace?
		if(grainPlayer){
			delete grainPlayer;
			grainPlayer = NULL;
		}

		sample = sampleIn;
		grainPlayer = new maxiGrainPlayer(sample);
	}

    maxi_float_t getNormalisedPosition() {
        return position / (maxi_float_t) sample->getLength();
    }

    maxi_float_t getPosition() {
        return position;
    }

    void setPosition(maxi_float_t pos) {
        position = pos * sample->getLength();
        position = maxiMap::clamp(position, 0, sample->getLength()-1);
    }


	//play at a speed
    inline maxi_float_t play(maxi_float_t speed=1, maxi_float_t grainLength=0.05, int overlaps=2, maxi_float_t posMod=0) {
		position = position + speed;
        looper++;
		if (position > sample->getLength()) position-= sample->getLength();
		if (position < 0) position += sample->getLength();
		maxi_float_t cycleLength = grainLength * maxiSettings::sampleRate  / overlaps;
        if (looper > cycleLength + randomOffset) {
            looper -= (cycleLength + randomOffset);
			speed = (speed > 0 ? 1 : -1);
			maxiGrain<F> *g = new maxiGrain<F>(sample, max(min((maxi_float_t)1.0,(maxi_float_t)(position / sample->getLength()) + posMod),(maxi_float_t)0.0f), grainLength, speed, &windowCache);
			grainPlayer->addGrain(g);
			randomOffset = rand() % 10;
		}
		return grainPlayer->play();
	}


    //provide your own position iteration
	inline maxi_float_t playAtPosition(maxi_float_t pos, maxi_float_t grainLength, int overlaps) {
		looper++;
		pos *= sample->getLength();
		if (0 == floor(fmod(looper, grainLength * maxiSettings::sampleRate / overlaps))) {
			maxiGrain<F> *g = new maxiGrain<F>(sample, max(min((maxi_float_t)1.0,(pos / sample->getLength())),(maxi_float_t)0.0f), grainLength, 1, &windowCache);
			grainPlayer->addGrain(g);
		}
		return grainPlayer->play();
	}
};

//in maxiTimeStretch, the speed is either 1 or -1, and the actual speed value only affects the grain position
//in maxiPitchShift, speed is uncoupled from position and allowed to set it's value incrementally, resulting in pitchshift.
//with both high speed values and negative speed values there are some terrific artefacts!

template<typename F>
class maxiPitchShift {
public:
	maxi_float_t position;
	long cycles;
	maxiSample *sample;
	maxiGrainPlayer *grainPlayer;
	maxiGrainWindowCache<F> windowCache;
	maxi_float_t randomOffset;

	maxiPitchShift(){
		position=0;
		cycles=0;
//		grainPlayer = new maxiGrainPlayer(sample);
		randomOffset=0;
	}

	maxiPitchShift(maxiSample *sample) : sample(sample) {
		position=0;
		cycles=0;
		grainPlayer = new maxiGrainPlayer(sample);
		randomOffset=0;
	}

	~maxiPitchShift() {
		delete grainPlayer;
	}

	void setSample(maxiSample* sampleIn){
		if(grainPlayer){
			delete grainPlayer;
			grainPlayer = NULL;
		}

		sample = sampleIn;
		grainPlayer = new maxiGrainPlayer(sample);
	}

	maxi_float_t play(maxi_float_t speed, maxi_float_t grainLength, int overlaps, maxi_float_t posMod=0.0f) {
		position = position + 1;
		cycles++;
		if (position > sample->getLength()) position=0;
		if (position < 0) position = sample->getLength();
		maxi_float_t cycleLength = grainLength * maxiSettings::sampleRate  / overlaps;
		maxi_float_t cycleMod = fmod(cycles, cycleLength + randomOffset);
		if (0 == floor(cycleMod)) {
			//			cout << cycleMod << endl;
			//speed = (speed > 0 ? 1 : -1);
			speed = speed - ((cycleMod / cycleLength) * 0.1f);
			maxiGrain<F> *g = new maxiGrain<F>(sample, max(min((maxi_float_t)1.0,(position / sample->getLength()) + posMod),(maxi_float_t)0.0f), grainLength, speed, &windowCache);
			grainPlayer->addGrain(g);
			//			cout << grainPlayer->grains.size() << endl;
			//			randomOffset = rand() % 10;
			//			randomOffset = rand() % 10;
		}
		return grainPlayer->play();
	}

};

//and here's maxiStretch. Args to the play function are basically speed for 'pitch' and rate for playback rate.
//the rest is the same.

template<typename F>
class maxiStretch {
public:
	maxi_float_t position;
	maxiSample *sample;
	maxiGrainPlayer *grainPlayer;
	maxiGrainWindowCache<F> windowCache;
	maxi_float_t randomOffset;
    unsigned  long loopStart, loopEnd, loopLength;
    maxi_float_t looper;

	maxiStretch(){
        randomOffset=0;
        loopStart = 0.0f;
        position=0;
        looper = 0;
        sample = NULL;
	}

	maxiStretch(maxiSample *sample) : sample(sample) {
		grainPlayer = new maxiGrainPlayer(sample);
		randomOffset=0;
        loopStart = 0.0f;
        loopEnd = sample->getLength();
        loopLength =sample->getLength();
		position=0;
        looper = 0;
	}

	void setSample(maxiSample* newSample){
		if(grainPlayer){
			delete grainPlayer;
			grainPlayer = NULL;
		}
        sample = newSample;
		grainPlayer = new maxiGrainPlayer(sample);
        loopStart = 0;
        loopEnd = sample->getLength();
        loopLength = sample->getLength();
        position=0;
        looper = 0;
	}

    maxi_float_t getNormalisedPosition() {
        return position / (maxi_float_t) sample->getLength();
    }

    maxi_float_t getPosition() {
        return position;
    }

    void setPosition(maxi_float_t pos) {
        position = pos * sample->getLength();
        position = maxiMap::clamp(position, 0, sample->getLength()-1);
    }

    void setLoopStart(maxi_float_t val) {
        loopStart = val * sample->getLength();
        loopLength = loopEnd - loopStart;
    }

    void setLoopEnd(maxi_float_t val) {
        loopEnd = val * sample->getLength();
        loopLength = loopEnd - loopStart;
    }

	~maxiStretch() {
		delete grainPlayer;
	}

    unsigned long getLoopEnd() {
        return loopEnd;
    }


	inline maxi_float_t play(maxi_float_t pitchstretch=1, maxi_float_t timestretch=1, maxi_float_t grainLength=0.05, int overlaps=2, maxi_float_t posMod=0.0f) {
        if (sample != NULL) {
            position = position + (1 * timestretch);
            looper++;
            if (position >= loopEnd) position-= loopLength;
            if (position < loopStart) position += loopLength;
            maxi_float_t cycleLength = grainLength * maxiSettings::sampleRate  / overlaps;
            if (looper > cycleLength + randomOffset) {
                looper -= (cycleLength + randomOffset);
                maxiGrain<F> *g = new maxiGrain<F>(sample, max(min((maxi_float_t)1.0,(position / sample->getLength()) + posMod),(maxi_float_t)0.0f), grainLength, pitchstretch, &windowCache);
                grainPlayer->addGrain(g);
                randomOffset = rand() % 10;
            }
            return grainPlayer->play();
        }else{
            return 0;
        }
	}

    inline maxi_float_t playAtPosition(maxi_float_t pitchstretch=1, maxi_float_t pos=0, maxi_float_t grainLength=0.05, int overlaps=2) {
        looper++;
        pos *= sample->getLength();
        if (0 == floor(fmod(looper, grainLength * maxiSettings::sampleRate / overlaps))) {
            maxiGrain<F> *g = new maxiGrain<F>(sample, max(min((maxi_float_t)1.0,(pos / sample->getLength())),(maxi_float_t)0.0f), grainLength, pitchstretch, &windowCache);
            grainPlayer->addGrain(g);
        }
        return grainPlayer->play();
    }


};

#endif
