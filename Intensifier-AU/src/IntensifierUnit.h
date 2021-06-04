#include "AUEffectBase.h"
#include "IntensifierUnitVersion.h"
#if AU_DEBUG_DISPATCHER
	#include "AUDebugDispatcher.h"
#endif
#ifndef __IntensifierUnit_h__
#define __IntensifierUnit_h__
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
// MARK: Cyclone RMS
#define AVERAGE_STACK    44100 //stack value
#define AVERAGE_MAXBUF  882000 //max buffer size
#define AVERAGE_DEFNPOINTS  100  /* CHECKME */
typedef struct {
    float x_accum; // sum
    float x_calib; // accumulator calibrator
    float *x_buf; // buffer pointer
    float x_stack[AVERAGE_STACK]; // buffer
    int x_alloc; // if x_buf is allocated or stack
    unsigned int x_count; // number of samples seen so far
    unsigned int x_npoints; // number of samples for moving average
    unsigned int x_sz; // allocated size for x_buf
    unsigned int x_bufrd; // readhead for buffer
    unsigned int x_max; // max size of buffer as specified by argument
} rmsaverage;
// MARK: Cyclone Slide
typedef struct {
    int x_slide_up;
    int x_slide_down;
    float x_last;
} slide;
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
