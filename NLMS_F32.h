#ifndef _NLMS_F32
#define _NLMS_F32

#include <Arduino.h>  //for Serial.println()
#include <arm_math.h> //ARM DSP extensions.  https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
#include "AudioStream_F32.h"



//#include "AudioLoopBack_F32.h" //form Tympan_Library


#ifndef MAX_FILT_LEN
#define MAX_FILT_LEN  1024//must be longer than afl
#endif


#define MU                  0.5f


/*
Send in reference and input signal, and it will hopefully find the filter between them

Right now it doesn't pipe data into the module

*/

class NLMS_F32 : public AudioStream_F32
{
	public:
		//constructor
	 	NLMS_F32(const AudioSettings_F32 &settings) : AudioStream_F32(2, inputQueueArray_f32) 
  		{
            block_size = settings.audio_block_samples;
         	filt_len = MAX_FILT_LEN;

            arm_lms_norm_init_f32(&S_lmsNorm_instance, filt_len, S, S_lmsStateF32, MU, block_size);
	  	}


      	int filt_len,block_size;

	    virtual void update(void);

      	void S_LMS(float32_t *input, float32_t *ref);


      	void printFilter(void) 
        {
            for(int i=filt_len; i >= 0; i--)
            {
                Serial.print(S[i], 3);
                Serial.print("  ");
            } 
        };

        void printError(void)
        {
            float total_error;

            arm_dot_prod_f32(err_signal, err_signal, block_size, &total_error);

            arm_sqrt_f32(total_error, &total_error);

            Serial.print("Error: ");
            Serial.print(total_error, 4);
            

        };

	protected:
        audio_block_f32_t *buffer_block = NULL;

		audio_block_f32_t *inputQueueArray_f32[2]; //memory pointer for the input to this module
        
        float32_t S[MAX_FILT_LEN];
        float32_t err_signal[MAX_FILT_LEN];

        
        float32_t S_lmsStateF32[MAX_FILT_LEN*2];
        arm_lms_norm_instance_f32 S_lmsNorm_instance;


    

};

#endif