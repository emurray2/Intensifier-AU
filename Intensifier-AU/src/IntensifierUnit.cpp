#include "IntensifierUnit.h"
/*
 The objects marked Cyclone were derived from the Max/MSP Cyclone library source code.
 The license for this code can be found below:

 --------------------------------------------------------------------------------------------------------------
 LICENSE.txt
 --------------------------------------------------------------------------------------------------------------

 Copyright (c) <2003-2020>, <Krzysztof Czaja, Fred Jan Kraan, Alexandre Porres, Derek Kwan, Matt Barber and others>
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the <organization> nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// MARK: BEGIN Cyclone RMS
void rmsaverage_zerobuf(rmsaverage *x) {
    unsigned int i;
    for (i=0; i < x->x_sz; i++) {
        x->x_buf[i] = 0.;
    };
}

void rmsaverage_reset(rmsaverage *x) {
    // clear buffer and reset everything to 0
    x->x_count = 0;
    x->x_accum = 0;
    x->x_bufrd = 0;
    rmsaverage_zerobuf(x);
}

void rmsaverage_sz(rmsaverage *x, unsigned int newsz) {
    // helper function to deal with allocation issues if needed
    int alloc = x->x_alloc;
    unsigned int cursz = x->x_sz; //current size
    // requested size
    if (newsz < 0) {
        newsz = 0;
    } else if (newsz > AVERAGE_MAXBUF) {
        newsz = AVERAGE_MAXBUF;
    };
    if (!alloc && newsz > AVERAGE_STACK) {
        x->x_buf = (float *)malloc(sizeof(float) * newsz);
        x->x_alloc = 1;
        x->x_sz = newsz;
    } else if (alloc && newsz > cursz) {
        x->x_buf = (float *)realloc(x->x_buf, sizeof(float) * newsz);
        x->x_sz = newsz;
    } else if (alloc && newsz < AVERAGE_STACK) {
        free(x->x_buf);
        x->x_sz = AVERAGE_STACK;
        x->x_buf = x->x_stack;
        x->x_alloc = 0;
    };
    rmsaverage_reset(x);
}

double rmsaverage_rmssum(float input, float accum, int add) {
    if (add) {
        accum += (input * input);
    } else {
        accum -= (input * input);
    };
    return (accum);
}

int rmsaverage_compute(rmsaverage *x, float *inSample, float *outSample) {
    unsigned int npoints = x->x_npoints;
    float result; // eventual result
    float input = *inSample;
    if (npoints > 1) {
        unsigned int bufrd = x->x_bufrd;
        // add input to accumulator
        x->x_accum = rmsaverage_rmssum(input, x->x_accum, 1);
        x->x_calib = rmsaverage_rmssum(input, x->x_calib, 1);
        unsigned int count = x->x_count;
        if(count < npoints) {
            // update count
            count++;
            x->x_count = count;
        } else {
            x->x_accum = rmsaverage_rmssum(x->x_buf[bufrd], x->x_accum, 0);
        };

        // overwrite/store current input value into buf
        x->x_buf[bufrd] = input;

        // calculate result
        result = x->x_accum/(float)npoints;
        result = sqrt(result);

        // incrementation step
        bufrd++;
        if (bufrd >= npoints) {
            bufrd = 0;
            x->x_accum = x->x_calib;
            x->x_calib = 0.0;
        };
        x->x_bufrd = bufrd;
    } else {
        result = fabs(input);
    }
    if (isnan(result))
        result = input;

    *outSample = result;
    return 1;
}

int rmsaverage_init(rmsaverage *x, unsigned int pointCount) {
    // default to stack for now...
    x->x_buf = x->x_stack;
    x->x_alloc = 0;
    x->x_sz = AVERAGE_STACK;

    //now allocate x_buf if necessary
    rmsaverage_sz(x, x->x_npoints);

    rmsaverage_reset(x);

    x->x_npoints = pointCount;
    return 1;
}

int rmsaverage_create(rmsaverage **x) {
    *x = (rmsaverage *)malloc(sizeof(rmsaverage));
    return 1;
}

int rmsaverage_destroy(rmsaverage **x) {
    free(*x);
    return 1;
}
// MARK: END Cyclone RMS
// MARK: BEGIN Cyclone Slide
int slide_compute(slide *x, float *inSample, float *outSample) {
    float last = x->x_last;
    float f = *inSample;
    float output = 0.0;
    if (f >= last) {
        if (x->x_slide_up > 1.)
            output = last + ((f - last) / x->x_slide_up);
        else
            output = last = f;
    } else if (f < last) {
        if (x->x_slide_down > 1)
            output = last + ((f - last) / x->x_slide_down);
        else
            output = last = f;
    }
    if (output == last && output != f)
        output = f;
    if (isnan(output))
        output = *inSample;

    *outSample = output;
    last = output;
    x->x_last = last;
    return 1;
}

void slide_reset(slide *x) {
    x->x_last = 0;
}

void slide_slide_up(slide *x, float f) {
    int i = (int)f;
    if (i > 1) {
        x->x_slide_up = i;
    } else {
        x->x_slide_up = 0;
    }
}

void slide_slide_down(slide *x, float f) {
    int i = (int)f;
    if (i > 1) {
        x->x_slide_down = i;
    } else {
        x->x_slide_down = 0;
    }
}

int slide_init(slide *x, float slideUpSamples, float slideDownSamples) {
    float f1 = slideUpSamples;
    float f2 = slideDownSamples;
    slide_slide_up(x, f1);
    slide_slide_down(x, f2);
    x->x_last = 0.;
    return 1;
}

int slide_create(slide **x) {
    *x = (slide *)malloc(sizeof(slide));
    return 1;
}

int slide_destroy(slide **x) {
    free(*x);
    return 1;
}
// MARK: END Cyclone Slide
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUDIOCOMPONENT_ENTRY(AUBaseFactory, IntensifierUnit)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::IntensifierUnit
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IntensifierUnit::IntensifierUnit (AudioUnit component) : AUEffectBase (component) {
    CreateElements ();
    Globals () -> UseIndexedParameters (kNumberOfParameters);
    SetParameter (
        kParameter_Input_Amount,
        kDefaultValue_Intensifier_Input_Amount
    );
    SetParameter (
        kParameter_Attack_Amount,
        kDefaultValue_Intensifier_Attack_Amount
    );
    SetParameter (
        kParameter_Release_Amount,
        kDefaultValue_Intensifier_Release_Amount
    );
    SetParameter (
        kParameter_Attack_Time,
        kDefaultValue_Intensifier_Attack_Time
    );
    SetParameter (
        kParameter_Release_Time,
        kDefaultValue_Intensifier_Release_Time
    );
    SetParameter (
        kParameter_Output_Amount,
        kDefaultValue_Intensifier_Output_Amount
    );
    SetAFactoryPresetAsCurrent (
        kPresets [kPreset_Default]
    );
    #if AU_DEBUG_DISPATCHER
        mDebugDispatcher = new AUDebugDispatcher (this);
    #endif
}
#pragma mark ____Parameters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::GetParameterInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult IntensifierUnit::GetParameterInfo (
        AudioUnitScope            inScope,
        AudioUnitParameterID    inParameterID,
        AudioUnitParameterInfo    &outParameterInfo
) {
    ComponentResult result = noErr;

    outParameterInfo.flags =
        kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable;

    if (inScope == kAudioUnitScope_Global) {
        switch (inParameterID) {

            case kParameter_Input_Amount:
                AUBase::FillInParameterName (
                    outParameterInfo,
                    kParamName_Intensifier_Input_Amount,
                    false
                );
                outParameterInfo.unit            = kAudioUnitParameterUnit_Decibels;
                outParameterInfo.minValue        = kMinimumValue_Intensifier_Input_Amount;
                outParameterInfo.maxValue        = kMaximumValue_Intensifier_Input_Amount;
                outParameterInfo.defaultValue    = kDefaultValue_Intensifier_Input_Amount;
                break;

            case kParameter_Attack_Amount:
                AUBase::FillInParameterName (
                    outParameterInfo,
                    kParamName_Intensifier_Attack_Amount,
                    false
                );
                outParameterInfo.unit            = kAudioUnitParameterUnit_Decibels;
                outParameterInfo.minValue        = kMinimumValue_Intensifier_Attack_Amount;
                outParameterInfo.maxValue        = kMaximumValue_Intensifier_Attack_Amount;
                outParameterInfo.defaultValue    = kDefaultValue_Intensifier_Attack_Amount;
                break;

            case kParameter_Release_Amount:
                AUBase::FillInParameterName (
                    outParameterInfo,
                    kParamName_Intensifier_Release_Amount,
                    false
                );
                outParameterInfo.unit            = kAudioUnitParameterUnit_Decibels;
                outParameterInfo.minValue        = kMinimumValue_Intensifier_Release_Amount;
                outParameterInfo.maxValue        = kMaximumValue_Intensifier_Release_Amount;
                outParameterInfo.defaultValue    = kDefaultValue_Intensifier_Release_Amount;
                break;

            case kParameter_Attack_Time:
                AUBase::FillInParameterName (
                    outParameterInfo,
                    kParamName_Intensifier_Attack_Time,
                    false
                );
                outParameterInfo.unit            = kAudioUnitParameterUnit_Milliseconds;
                outParameterInfo.minValue        = kMinimumValue_Intensifier_Attack_Time;
                outParameterInfo.maxValue        = kMaximumValue_Intensifier_Attack_Time;
                outParameterInfo.defaultValue    = kDefaultValue_Intensifier_Attack_Time;
                break;

            case kParameter_Release_Time:
                AUBase::FillInParameterName (
                    outParameterInfo,
                    kParamName_Intensifier_Release_Time,
                    false
                );
                outParameterInfo.unit            = kAudioUnitParameterUnit_Seconds;
                outParameterInfo.minValue        = kMinimumValue_Intensifier_Release_Time;
                outParameterInfo.maxValue        = kMaximumValue_Intensifier_Release_Time;
                outParameterInfo.defaultValue    = kDefaultValue_Intensifier_Release_Time;
                break;

            case kParameter_Output_Amount:
                AUBase::FillInParameterName (
                    outParameterInfo,
                    kParamName_Intensifier_Output_Amount,
                    false
                );
                outParameterInfo.unit            = kAudioUnitParameterUnit_Decibels;
                outParameterInfo.minValue        = kMinimumValue_Intensifier_Output_Amount;
                outParameterInfo.maxValue        = kMaximumValue_Intensifier_Output_Amount;
                outParameterInfo.defaultValue    = kDefaultValue_Intensifier_Output_Amount;
                break;

            default:
                result = kAudioUnitErr_InvalidParameter;
                break;
        }
    } else {
        result = kAudioUnitErr_InvalidParameter;
    }
    return result;
}
#pragma mark ____Properties
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::GetPropertyInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult IntensifierUnit::GetPropertyInfo (
    AudioUnitPropertyID    inID,
    AudioUnitScope        inScope,
    AudioUnitElement    inElement,
    UInt32                &outDataSize,
    Boolean                &outWritable
) {
    return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::GetProperty
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult IntensifierUnit::GetProperty (
    AudioUnitPropertyID inID,
    AudioUnitScope         inScope,
    AudioUnitElement     inElement,
    void                *outData
) {
    return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}
#pragma mark ____Factory Presets
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::GetPresets
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult IntensifierUnit::GetPresets (
    CFArrayRef    *outData
) const {

    if (outData == NULL) return noErr;

    CFMutableArrayRef presetsArray = CFArrayCreateMutable (
        NULL,
        kNumberPresets,
        NULL
    );

    for (int i = 0; i < kNumberPresets; ++i) {
        CFArrayAppendValue (
            presetsArray,
            &kPresets [i]
        );
    }

    *outData = (CFArrayRef) presetsArray;
    return noErr;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::NewFactoryPresetSet
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus IntensifierUnit::NewFactoryPresetSet (
    const AUPreset &inNewFactoryPreset
) {
    SInt32 chosenPreset = inNewFactoryPreset.presetNumber;

    if (
        chosenPreset == kPreset_Subtle
    ) {
        for (int i = 0; i < kNumberPresets; ++i) {
            if (chosenPreset == kPresets[i].presetNumber) {
                switch (chosenPreset) {
                    case kPreset_Subtle:
                        SetParameter (
                            kParameter_Input_Amount,
                            kParameter_Preset_Input_Amount_Subtle
                        );
                        SetParameter (
                            kParameter_Attack_Amount,
                            kParameter_Preset_Attack_Amount_Subtle
                        );
                        SetParameter (
                            kParameter_Release_Amount,
                            kParameter_Preset_Release_Amount_Subtle
                        );
                        SetParameter (
                            kParameter_Attack_Time,
                            kParameter_Preset_Attack_Time_Subtle
                        );
                        SetParameter (
                            kParameter_Release_Time,
                            kParameter_Preset_Release_Time_Subtle
                        );
                        SetParameter (
                            kParameter_Output_Amount,
                            kParameter_Preset_Output_Amount_Subtle
                        );
                        break;
                }

                // Updates the preset menu in the generic view to display the new factory preset.
                SetAFactoryPresetAsCurrent (
                    kPresets [i]
                );
                return noErr;
            }
        }
    }
    return kAudioUnitErr_InvalidProperty;
}
#pragma mark ____IntensifierUnitEffectKernel
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::IntensifierUnitKernel::IntensifierUnitKernel()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IntensifierUnit::IntensifierUnitKernel::IntensifierUnitKernel (AUEffectBase *inAudioUnit ) : AUKernelBase (inAudioUnit),
    mSamplesProcessed (0)
{
    mSampleFrequency = GetSampleRate ();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::IntensifierUnitKernel::Reset()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IntensifierUnit::IntensifierUnitKernel::Reset() {
    mSamplesProcessed    = 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    IntensifierUnit::IntensifierUnitKernel::Process
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void IntensifierUnit::IntensifierUnitKernel::Process (
    const Float32     *inSourceP,            // The audio sample input buffer.
    Float32             *inDestP,            // The audio sample output buffer.
    UInt32             inSamplesToProcess,    // The number of samples in the input buffer.
    UInt32            inNumChannels,        // The number of input channels. This is always equal to 1
                                        //   because there is always one kernel object instantiated
                                        //   per channel of audio.
    bool            &ioSilence            // A Boolean flag indicating whether the input to the audio
                                        //   unit consists of silence, with a TRUE value indicating
                                        //   silence.
) {
    // Ignores the request to perform the Process method if the input to the audio unit is silence.
    if (!ioSilence) {

        // Assigns a pointer variable to the start of the audio sample input buffer.
        const Float32 *sourceP = inSourceP;

        // Assigns a pointer variable to the start of the audio sample output buffer.
        Float32    *destP = inDestP,
                inputSample,            // The current audio sample to process.
                outputSample,            // The current audio output sample resulting from one iteration of the
                                        //   processing loop.
                IntensifierInputAmount,
                IntensifierAttackAmount,
                IntensifierReleaseAmount,
                IntensifierAttackTime,
                IntensifierReleaseTime,
                IntensifierOutputAmount;

        IntensifierInputAmount = GetParameter (kParameter_Input_Amount);
        IntensifierAttackAmount = GetParameter (kParameter_Attack_Amount);
        IntensifierReleaseAmount = GetParameter (kParameter_Release_Amount);
        IntensifierAttackTime = GetParameter (kParameter_Attack_Time);
        IntensifierReleaseTime = GetParameter (kParameter_Release_Time);
        IntensifierOutputAmount = GetParameter (kParameter_Output_Amount);

        // Performs bounds checking on the parameters.
        if (IntensifierInputAmount    < kMinimumValue_Intensifier_Input_Amount)
            IntensifierInputAmount    = kMinimumValue_Intensifier_Input_Amount;
        if (IntensifierInputAmount    > kMaximumValue_Intensifier_Input_Amount)
            IntensifierInputAmount    = kMaximumValue_Intensifier_Input_Amount;

        if (IntensifierAttackAmount    < kMinimumValue_Intensifier_Attack_Amount)
            IntensifierAttackAmount    = kMinimumValue_Intensifier_Attack_Amount;
        if (IntensifierAttackAmount    > kMaximumValue_Intensifier_Attack_Amount)
            IntensifierAttackAmount    = kMaximumValue_Intensifier_Attack_Amount;

        if (IntensifierReleaseAmount    < kMinimumValue_Intensifier_Release_Amount)
            IntensifierReleaseAmount    = kMinimumValue_Intensifier_Release_Amount;
        if (IntensifierReleaseAmount    > kMaximumValue_Intensifier_Release_Amount)
            IntensifierReleaseAmount    = kMaximumValue_Intensifier_Release_Amount;

        if (IntensifierAttackTime    < kMinimumValue_Intensifier_Attack_Time)
            IntensifierAttackTime    = kMinimumValue_Intensifier_Attack_Time;
        if (IntensifierAttackTime    > kMaximumValue_Intensifier_Attack_Time)
            IntensifierAttackTime    = kMaximumValue_Intensifier_Attack_Time;

        if (IntensifierReleaseTime    < kMinimumValue_Intensifier_Release_Time)
            IntensifierReleaseTime    = kMinimumValue_Intensifier_Release_Time;
        if (IntensifierReleaseTime    > kMaximumValue_Intensifier_Release_Time)
            IntensifierReleaseTime    = kMaximumValue_Intensifier_Release_Time;

        if (IntensifierOutputAmount    < kMinimumValue_Intensifier_Output_Amount)
            IntensifierOutputAmount    = kMinimumValue_Intensifier_Output_Amount;
        if (IntensifierOutputAmount    > kMaximumValue_Intensifier_Output_Amount)
            IntensifierOutputAmount    = kMaximumValue_Intensifier_Output_Amount;

        // The sample processing loop: processes the current batch of samples, one sample at a time.
        for (int i = inSamplesToProcess; i > 0; --i) {
            // Gets the next input sample.
            inputSample            = *sourceP;

            // Calculates the next output sample.
            outputSample        = (inputSample * powf(10., IntensifierInputAmount / 20.0));

            // Stores the output sample in the output buffer.
            *destP                = outputSample;

            // Advances to the next sample location in the input and output buffers.
            sourceP                += 1;
            destP                += 1;

            // Increments the global samples counter.
            mSamplesProcessed    += 1;
        }
    }
}
