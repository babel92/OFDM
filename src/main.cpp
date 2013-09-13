#include "global.h"
#include "audio.h"
#include <iostream>
#include "plotter.h"


using namespace std;

int RecCallback(const void*input,void*output,int framecount,PaTime timespan,void*userdata)
{

    Plotter*plt=(Plotter*)userdata;

    plt->PlotSpectrum((Sample*)input,framecount);
    return paContinue;
}



int main()
{
    /*
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
    Plotter plt;
    Init_Portaudio();

    Init_Portaudio_Record(RecCallback,&plt);
    while(1)
        Pa_Sleep(5000);
    Cleanup_Portaudio();

    return 0;
}
