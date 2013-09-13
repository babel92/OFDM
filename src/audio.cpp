#include "audio.h"
#include "global.h"
#include "portaudio/portaudio.h"

DataCallback UserRecordCallback;
PaStream *RecordStream;
PaTime LastTime;

int Pa_RecordStream( const void *input,
                     void *output,
                     unsigned long frameCount,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags,
                     void *userData )
{
    //printf("fc=%d in=%f out=%f curr=%f\n",frameCount,timeInfo->inputBufferAdcTime,timeInfo->outputBufferDacTime,timeInfo->currentTime);
    return UserRecordCallback(input,output,frameCount,0,userData);
}


void Init_Portaudio()
{
    PaError err=Pa_Initialize();
    if(paNoError!=err)
    {
        Err_printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        exit(1);
    }
}

void Init_Portaudio_Record(DataCallback cutecallback,void*userdata)
{
    PaError err;

    UserRecordCallback=cutecallback;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &RecordStream,
                                1,          /* no input channels */
                                0,          /* stereo output */
                                paFloat32,
                                44100,
                                256,        /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                Pa_RecordStream, /* this is your callback function */
                                userdata); /*This is a pointer that will be passed to
                                                   your callback*/
    LastTime=Pa_GetStreamTime(RecordStream);
    Pa_StartStream(RecordStream);
}

void Cleanup_Portaudio()
{
    PaError err = Pa_Terminate();
    if( err != paNoError )
    {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    }
}

