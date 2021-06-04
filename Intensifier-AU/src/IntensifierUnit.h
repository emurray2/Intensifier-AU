#include "AUEffectBase.h"
#include "IntensifierUnitVersion.h"
#if AU_DEBUG_DISPATCHER
	#include "AUDebugDispatcher.h"
#endif
#ifndef __IntensifierUnit_h__
#define __IntensifierUnit_h__
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Constants for parameters and  factory presets
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ____IntensifierUnit Parameter Constants
static CFStringRef kParamName_Intensifier_Input_Amount = CFSTR ("Input Amount");
static const float kDefaultValue_Intensifier_Input_Amount = 0.0;
static const float kMinimumValue_Intensifier_Input_Amount = -60.0;
static const float kMaximumValue_Intensifier_Input_Amount = 30.0;

static CFStringRef kParamName_Intensifier_Attack_Amount = CFSTR ("Attack Amount");
static const float kDefaultValue_Intensifier_Attack_Amount = 0.0;
static const float kMinimumValue_Intensifier_Attack_Amount = -40.0;
static const float kMaximumValue_Intensifier_Attack_Amount = 30.0;

static CFStringRef kParamName_Intensifier_Release_Amount = CFSTR ("Release Amount");
static const float kDefaultValue_Intensifier_Release_Amount = 0.0;
static const float kMinimumValue_Intensifier_Release_Amount = -60.0;
static const float kMaximumValue_Intensifier_Release_Amount = 30.0;

static CFStringRef kParamName_Intensifier_Attack_Time = CFSTR ("Attack Time");
static const float kDefaultValue_Intensifier_Attack_Time = 0.0;
static const float kMinimumValue_Intensifier_Attack_Time = 0.0;
static const float kMaximumValue_Intensifier_Attack_Time = 500.0;

static CFStringRef kParamName_Intensifier_Release_Time = CFSTR ("Release Time");
static const float kDefaultValue_Intensifier_Release_Time = 0.0;
static const float kMinimumValue_Intensifier_Release_Time = 0.0;
static const float kMaximumValue_Intensifier_Release_Time = 5.0;

static CFStringRef kParamName_Intensifier_Output_Amount = CFSTR ("Output Amount");
static const float kDefaultValue_Intensifier_Output_Amount = 0.0;
static const float kMinimumValue_Intensifier_Output_Amount = -60.0;
static const float kMaximumValue_Intensifier_Output_Amount = 30.0;

enum {
	kParameter_Input_Amount	= 0,
	kParameter_Attack_Amount = 1,
	kParameter_Release_Amount = 2,
    kParameter_Attack_Time = 3,
    kParameter_Release_Time = 4,
    kParameter_Output_Amount = 5,
	kNumberOfParameters = 6
};

#pragma mark ____IntensifierUnit Factory Preset Constants
static const float kParameter_Preset_Input_Amount_Subtle = 0.0;
static const float kParameter_Preset_Attack_Amount_Subtle = -10.0;
static const float kParameter_Preset_Release_Amount_Subtle = 5.0;
static const float kParameter_Preset_Attack_Time_Subtle = 20.0;
static const float kParameter_Preset_Release_Time_Subtle = 1.0;
static const float kParameter_Preset_Output_Amount_Subtle = 0.0;

enum {
	kPreset_Subtle	= 0,
	kNumberPresets	= 1
};

static AUPreset kPresets [kNumberPresets] = {
	{kPreset_Subtle, CFSTR ("Subtle")}
};

static const int kPreset_Default = kPreset_Subtle;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IntensifierUnit class
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#pragma mark ____IntensifierUnit
class IntensifierUnit : public AUEffectBase {

public:
	IntensifierUnit (AudioUnit component);
	
#if AU_DEBUG_DISPATCHER
	virtual ~IntensifierUnit () {delete mDebugDispatcher;}
#endif
	
	virtual AUKernelBase *NewKernel () {return new IntensifierUnitKernel(this);}
    
	virtual	ComponentResult GetParameterInfo (
		AudioUnitScope			inScope,
		AudioUnitParameterID	inParameterID,
		AudioUnitParameterInfo	&outParameterInfo
	);
    
	virtual ComponentResult GetPropertyInfo (
		AudioUnitPropertyID		inID,
		AudioUnitScope			inScope,
		AudioUnitElement		inElement,
		UInt32					&outDataSize,
		Boolean					&outWritable
	);
	
	virtual ComponentResult GetProperty (
		AudioUnitPropertyID		inID,
		AudioUnitScope			inScope,
		AudioUnitElement		inElement,
		void					*outData
	);

	virtual	bool SupportsTail () {return true;}

	virtual ComponentResult	Version () {return kIntensifierUnitVersion;}

    virtual ComponentResult	GetPresets (
		CFArrayRef	*outData
	) const;

    virtual OSStatus NewFactoryPresetSet (
		const AUPreset	&inNewFactoryPreset
	);

protected:
	class IntensifierUnitKernel : public AUKernelBase {
		public:
			IntensifierUnitKernel (AUEffectBase *inAudioUnit);

			virtual void Process (
				const Float32 	*inSourceP,
				Float32		 	*inDestP,
				UInt32 			inFramesToProcess,
				UInt32			inNumChannels, // equal to 1
				bool			&ioSilence
		);
		
        virtual void Reset ();
		
		private:
			Float32 mSampleFrequency;			// The "sample rate" of the audio signal being processed
			long	mSamplesProcessed;			// The number of samples processed since the audio unit
												//   started rendering or since this variable was last
												//   reset to 0. We have to keep track of this because
												//   we vary the Intensifier continuously and independently
												//   of the input buffer size.
			enum	{sampleLimit = (int) 10E6};	// To keep the value of mSamplesProcessed within a 
												//   reasonable limit. 10E6 is equivalent to the number   
												//   of samples in 100 seconds of 96 kHz audio.
	};
};
#endif
