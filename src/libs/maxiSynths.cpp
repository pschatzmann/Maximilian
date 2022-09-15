//
//  maxiSynths.cpp
//  granular
//
//  Created by Michael Grierson on 16/08/2015.
//
//

#include "maxiSynths.h"

maxiKick::maxiKick(){

    maxiKick::envelope.setAttack(0);
    maxiKick::setPitch(200);
    maxiKick::envelope.setDecay(1);
    maxiKick::envelope.setSustain(1);
    maxiKick::envelope.setRelease(500);
    maxiKick::envelope.holdtime=1;
    maxiKick::envelope.trigger=0;

};

maxi_float_t maxiKick::play(){

    envOut=envelope.adsr(1.,envelope.trigger);

    if (inverse) {

        envOut=fabs(1-envOut);

    }

    output=kick.sinewave(pitch*envOut)*envOut;

    if (envelope.trigger==1) {
        envelope.trigger=0;
    }

    if (useDistortion) {

        output=distort.fastAtanDist(output, distortion);
    }

    if (useFilter) {

        output=filter.lores(output, cutoff, resonance);

    }

    if (useLimiter) {

        if (output*gain > 1) {

            return 1.;

        } else if (output*gain < -1) {

            return -1.;

        } else {

            return output*gain;

        }



    } else {

        return output*gain;

    }
};

void maxiKick::setRelease(maxi_float_t release) {

    envelope.setRelease(release);

}

// void maxiKick::setPitch(maxi_float_t newPitch) {
//
//     pitch=newPitch;
//
// }
//
void maxiKick::trigger() {

    envelope.trigger=1;

}

maxiSnare::maxiSnare(){

    maxiSnare::envelope.setAttack(0);
    maxiSnare::setPitch(800);
    maxiSnare::envelope.setDecay(20);
    maxiSnare::envelope.setSustain(0.05);
    maxiSnare::envelope.setRelease(300);
    maxiSnare::envelope.holdtime=1;
    maxiSnare::envelope.trigger=0;

};

maxi_float_t maxiSnare::play(){

    envOut=envelope.adsr(1.,envelope.trigger);

    if (inverse) {

        envOut=fabs(1-envOut);

    }

    output=(tone.triangle(pitch*(0.1f+(envOut*0.85f)))+noise.noise())*envOut;

    if (envelope.trigger==1) {
        envelope.trigger=0;
    }

    if (useDistortion) {

        output=distort.fastAtanDist(output, distortion);
    }

    if (useFilter) {

        output=filter.lores(output, cutoff, resonance);

    }

    if (useLimiter) {

        if (output*gain > 1) {

            return 1.;

        } else if (output*gain < -1) {

            return -1.;

        } else {

            return output*gain;

        }



    } else {

        return output*gain;

    }

};

void maxiSnare::setRelease(maxi_float_t release) {

    envelope.setRelease(release);

}

void maxiSnare::setPitch(maxi_float_t newPitch) {

    pitch=newPitch;

}

void maxiSnare::trigger() {

    envelope.trigger=1;

}

maxiHats::maxiHats(){

    maxiHats::envelope.setAttack(0);
    maxiHats::setPitch(12000);
    maxiHats::envelope.setDecay(20);
    maxiHats::envelope.setSustain(0.1);
    maxiHats::envelope.setRelease(300);
    maxiHats::envelope.holdtime=1;
    maxiHats::envelope.trigger=0;
    maxiHats::filter.setCutoff(8000);
    maxiHats::filter.setResonance(1);

};

maxi_float_t maxiHats::play(){

    envOut=envelope.adsr(1.,envelope.trigger);

    if (inverse) {

        envOut=fabs(1-envOut);

    }

    output=(tone.sinebuf(pitch)+noise.noise())*envOut;

    if (envelope.trigger==1) {
        envelope.trigger=0;
    }

    if (useDistortion) {

        output=distort.fastAtanDist(output, distortion);
    }

    if (useFilter) {

        output=filter.play(output, 0., 0., 1., 0.);

    }

    if (useLimiter) {

        if (output*gain > 1) {

            return 1.;

        } else if (output*gain < -1) {

            return -1.;

        } else {

            return output*gain;

        }



    } else {

        return output*gain;

    }

};

void maxiHats::setRelease(maxi_float_t release) {

    envelope.setRelease(release);

}

void maxiHats::setPitch(maxi_float_t newPitch) {

    pitch=newPitch;

}

void maxiHats::trigger() {

    envelope.trigger=1;

}


maxiSampler::maxiSampler() {

    maxiSampler::voices=32;
    maxiSampler::currentVoice=0;


    for (int i=0;i<voices;i++) {

        maxiSampler::envelopes[i].setAttack(0);
        maxiSampler::envelopes[i].setDecay(1);
        maxiSampler::envelopes[i].setSustain(1.);
        maxiSampler::envelopes[i].setRelease(2000);
        maxiSampler::envelopes[i].holdtime=1;
        maxiSampler::envelopes[i].trigger=0;
        maxiSampler::envOut[i]=0;
        maxiSampler::pitch[i]=0;
        maxiSampler::outputs[i]=0;


    }
}

void maxiSampler::setNumVoices(int numVoices) {

    voices=numVoices;

}

maxi_float_t maxiSampler::play() {

    output=0;

    for (int i=0;i<voices;i++) {

            envOut[i]=envelopes[i].adsr(envOutGain[i],envelopes[i].trigger);

          if (envOut[i]>0.f) {
              outputs[i]=samples[i].playOnZXAtSpeedFromOffset(pitchRatios[(int)pitch[i]+originalPitch]*((1.f/samples[i].getLength())*maxiSettings::sampleRate),0,samples[i].getLength())*envOut[i];
            output+=outputs[i]/voices;

            if (envelopes[i].trigger==1 && !sustain) {
                envelopes[i].trigger=0;

            }

        }

    } return output;

}

void maxiSampler::load(string inFile, bool setall) {

    if (setall) {
        for (int i=0;i<voices;i++) {

            samples[i].load(inFile);

        }

    } else {

        samples[currentVoice].load(inFile);

    }


}

void maxiSampler::setPitch(maxi_float_t pitchIn, bool setall) {

    if (setall) {
        for (int i=0;i<voices;i++) {

            pitch[i]=pitchIn;

        }

    } else {

    pitch[currentVoice]=pitchIn;

    }

}

void maxiSampler::midiNoteOn(maxi_float_t pitchIn, maxi_float_t velocity, bool setall) {

    if (setall) {
        for (int i=0;i<voices;i++) {

            pitch[i]=pitchIn;

        }

    } else {

            pitch[currentVoice]=pitchIn;
            envOutGain[currentVoice]=velocity/128;

    }

}

void maxiSampler::midiNoteOff(maxi_float_t pitchIn, maxi_float_t velocity, bool setall) {


        for (int i=0;i<voices;i++){

            if (pitch[i]==pitchIn) {

                envelopes[i].trigger=0;

        }

    }
}


void maxiSampler::setAttack(maxi_float_t attackD, bool setall) {

    if (setall) {

        for (int i=0;i<voices;i++) {

            envelopes[i].setAttack(attackD);

        }

    } else {

        envelopes[currentVoice].setAttack(attackD);


    }


}

void maxiSampler::setDecay(maxi_float_t decayD, bool setall) {

    if (setall) {

        for (int i=0;i<voices;i++) {

            envelopes[i].setDecay(decayD);

        }

    } else {

        envelopes[currentVoice].setDecay(decayD);


    }


}

void maxiSampler::setSustain(maxi_float_t sustainD, bool setall) {

    if (setall) {

        for (int i=0;i<voices;i++) {

            envelopes[i].setSustain(sustainD);

        }

    } else {

        envelopes[currentVoice].setSustain(sustainD);


    }


}

void maxiSampler::setRelease(maxi_float_t releaseD, bool setall) {

    if (setall) {

        for (int i=0;i<voices;i++) {

            envelopes[i].setRelease(releaseD);

        }

    } else {

        envelopes[currentVoice].setRelease(releaseD);


    }


}

void maxiSampler::setPosition(maxi_float_t positionD, bool setall){

    if (setall) {

        for (int i=0;i<voices;i++) {

            samples[i].setPosition(positionD);

        }

    } else {

        samples[currentVoice].setPosition(positionD);


    }


}

void maxiSampler::trigger() {

    envelopes[currentVoice].trigger=1;
    samples[currentVoice].trigger();
    currentVoice++;
    currentVoice=currentVoice%voices;

}
