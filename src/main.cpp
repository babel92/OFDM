#include "global.h"
#include "audio.h"
#include <iostream>
#include "plotter.h"


using namespace std;

int RecCallback(const void*input,void*output,int framecount,PaTime timespan,void*userdata)
{
    Sample buf[FRAME_SIZE*2];
    RtlZeroMemory(buf,1024*sizeof(Sample));
    Plotter**plt=(Plotter**)userdata;
    plt[0]->Plot((Real*)input,framecount);
    FFT_R2Mod((Real*)input,buf,framecount);
    plt[1]->Plot((Real*)buf,framecount/2);
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
    Plotter plt1,*plt[2],plt2;
    plt[0]=&plt1;
    plt[1]=&plt2;
    plt[0]->SetTitle("Waveform");
    plt[0]->SetXMax(FRAME_SIZE);
    plt[0]->SetXMin(-10);
    plt[0]->SetYMax(1);
    plt[0]->SetYMin(-1);
    plt[0]->SetXText("Sample");
    plt[0]->SetYText("Amplitude");

    plt[1]->SetTitle("Spectrum");
    plt[1]->SetXMax(FRAME_SIZE/2);
    plt[1]->SetXMin(0);
    plt[1]->SetYMax(40);
    plt[1]->SetYMin(0);
    Init_Portaudio();

    Init_Portaudio_Record(RecCallback,plt);

    while(1)
        Pa_Sleep(5000);
    Cleanup_Portaudio();

    return 0;
}
