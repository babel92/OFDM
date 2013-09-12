#include <iostream>
#include <cstdlib>
#include "fftw/include/fftw3.h"
#include "portaudio/include/portaudio.h"

using namespace std;

typedef
#ifdef USE_DOUBLE
double
#else
float
#endif
Real;

int lineno;

float frand()
{
    float r = (float)rand()/(float)RAND_MAX;
    return r;
}
void Init_Portaudio()
{
    PaError err=Pa_Initialize();
    if(paNoError!=err)
    {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        exit(1);
    }
}

void Cleanup_Portaudio()
{
    PaError err = Pa_Terminate();
    if( err != paNoError )
    {
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    }
}

struct PaData
{
    Real left;
    Real right;
};

int Pa_Stream( const void *input,
               void *output,
               unsigned long frameCount,
               const PaStreamCallbackTimeInfo* timeInfo,
               PaStreamCallbackFlags statusFlags,
               void *userData )
{
    PaData*ptr=(PaData*)input;

    for(int i=0; i<frameCount; i++ )
    {
        printf("%d:%f %f\n",++lineno,ptr->left,ptr->right);
        ptr+=1;
    }
    return paContinue;
}

int main()
{/*
    fftw_complex *out;
    fftw_plan plan;

    double arr[]={1,2,3,4,5,6};
    int n = sizeof(arr)/sizeof(double);

    out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * n );
    plan = fftw_plan_dft_r2c_1d ( n, arr, out, FFTW_ESTIMATE );
    fftw_execute ( plan );

    for (int i = 0; i < n; i++ )
    {
        printf ( "  %3d  %12lf  %12lf\n", i, out[i][0], out[i][1] );
    }

    fftw_free(out);
    fftw_destroy_plan(plan);*/
    Init_Portaudio();


    PaStream *stream;
    PaError err;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
                                2,          /* no input channels */
                                0,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                44100,
                                256,        /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                Pa_Stream, /* this is your callback function */
                                NULL); /*This is a pointer that will be passed to
                                                   your callback*/
    err = Pa_StartStream( stream );
    Pa_Sleep(5000);

    Cleanup_Portaudio();
    return 0;
}
