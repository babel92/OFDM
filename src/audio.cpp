#include "audio.h"
#include "global.h"
#include "portaudio/portaudio.h"
#include "fftw/fftw3.h"
#include <math.h>
#include <cstring>

DataCallback UserRecordCallback;
DataCallback UserPlayCallback;
PaStream *RecordStream;
PaStream *PlaybackStream;
PaTime LastTime;

int Initialized = 0;

int Pa_RecordStream( const void *input,
                     void *output,
                     unsigned long frameCount,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags,
                     void *userData )
{
    NOT_USED(timeInfo);
    NOT_USED(statusFlags);
    //printf("fc=%d in=%f out=%f curr=%f\n",frameCount,timeInfo->inputBufferAdcTime,timeInfo->outputBufferDacTime,timeInfo->currentTime);
    return UserRecordCallback(input,output,frameCount,0,userData);
}

int Pa_PlayStream(const void *input,
	void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	NOT_USED(timeInfo);
	NOT_USED(statusFlags);
	//printf("fc=%d in=%f out=%f curr=%f\n",frameCount,timeInfo->inputBufferAdcTime,timeInfo->outputBufferDacTime,timeInfo->currentTime);
	return UserPlayCallback(input, output, frameCount, 0, userData);
}

void Init_Portaudio()
{
	if (Initialized)
		return;
    PaError err=Pa_Initialize();
    if(paNoError!=err)
    {
        Err_printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        exit(1);
    }
	Initialized = 1;
}

/** This function has static ptrs which are initialized by arg size
 *  during the first call, so never use changing size
 */
void FFT_R2Mod(Real*input,Real*output,int size)
{
    static fftwf_complex*mem=(fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*size);
    static fftwf_plan plan=fftwf_plan_dft_r2c_1d(size,input,mem,FFTW_ESTIMATE);

    fftwf_execute(plan);
    for(int i=0;i<size/2;++i)
        output[i]=sqrt(mem[i][0]*mem[i][0]+mem[i][1]*mem[i][1]);
}

void Init_Portaudio_Record(DataCallback cutecallback,void*userdata)
{
    PaError err;

    UserRecordCallback=cutecallback;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &RecordStream,
                                1,        
                                0,        
                                paFloat32,
                                SAMPLE_RATE,
                                FRAME_SIZE,
                                cutecallback?Pa_RecordStream:NULL,
                                cutecallback?userdata:NULL); /*This is a pointer that will be passed to
                                                   your callback*/
    LastTime=Pa_GetStreamTime(RecordStream);
    Pa_StartStream(RecordStream);
}

void Init_Portaudio_Play(DataCallback cutecallback, void*userdata)
{
	PaError err;

	UserPlayCallback = cutecallback;
	/* Open an audio I/O stream. */
	err = Pa_OpenDefaultStream(&PlaybackStream,
		0,          /* input channels */
		1,          /* stereo output */
		paFloat32,
		SAMPLE_RATE,
		FRAME_SIZE,
		cutecallback ? Pa_PlayStream : NULL,
		cutecallback ? userdata : NULL); /*This is a pointer that will be passed to
										 your callback*/
	LastTime = Pa_GetStreamTime(PlaybackStream);
	Pa_StartStream(PlaybackStream);
}

void Audio_Read(void*Output,int Size)
{
    Pa_ReadStream(RecordStream,Output,Size);
}

void Audio_Write(void*Input, int Size)
{
	Pa_WriteStream(PlaybackStream, Input, Size);
}

void Cleanup_Portaudio()
{
    PaError err = Pa_Terminate();
    if( err != paNoError )
    {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    }
}

