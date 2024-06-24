#include "NLMS_F32.h"
//#include <cfloat> //for "isfinite()"
//#include <cmath>  //actually, this one is for "isfinite()" ?


//here's the method that is called automatically by the Teensy Audio Library
void NLMS_F32::update(void) 
{

	//receive the input audio data
  	audio_block_f32_t *noise_block = AudioStream_F32::receiveReadOnly_f32(0);
  	if (!noise_block ) return;

    audio_block_f32_t *mic_block = AudioStream_F32::receiveReadOnly_f32(1);
    if (!mic_block )return;

    if (!buffer_block) buffer_block = allocate_f32();
    else{S_LMS(buffer_block->data, mic_block->data); }

    for (int i=0; i < noise_block->length; i++) 
    {
        buffer_block->data[i] = noise_block->data[i];
        
    }


    AudioStream_F32::release(mic_block);
    AudioStream_F32::release(noise_block);


}

// The input is noise, the reference input is the filtered noise
void NLMS_F32::S_LMS( float32_t *input,float32_t *ref)
{
    float32_t dummy[block_size];
    arm_lms_norm_f32(&S_lmsNorm_instance, /* LMSNorm instance */
         input,                         /* Input signal */
         ref,                         /* Reference Signal */
         dummy,                         /* Converged Signal */
         err_signal,                    /* Error Signal, this will become small as the signal converges */
         block_size);                    /* BlockSize */
}