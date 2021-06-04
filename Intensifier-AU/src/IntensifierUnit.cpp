#include "IntensifierUnit.h"
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
