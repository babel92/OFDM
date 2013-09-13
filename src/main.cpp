#include "global.h"
#include "audio.h"
#include <iostream>
#include "mgl2/qt.h"

using namespace std;

mglGraph gr;

int RecCallback(const void*input,void*output,int framecount,PaTime timespan,void*userdata)
{
    gr.NewFrame();
    gr.Box();
    mglData data(framecount);
    data.Set((float*)input,framecount);
    gr.Plot(data,"b");
    gr.EndFrame();
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

    gr.StartGIF("sample.gif",16);

    Init_Portaudio();

    Init_Portaudio_Record(RecCallback,NULL);

    Pa_Sleep(5000);
    Cleanup_Portaudio();
    gr.CloseGIF();
    return 0;
}
