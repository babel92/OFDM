#include "global.h"
#include "audio.h"
#include <iostream>
#include "plotter.h"
#include "safecall.h"

using namespace std;

int RecCallback(const void*input,void*output,int framecount,PaTime timespan,void*userdata)
{
    static Sample axis[FRAME_SIZE*2]={0};
    Sample buf[FRAME_SIZE];
    static bool init=0;
    if(!init)
    {
        for(int i=0;i<FRAME_SIZE;++i)
        {
            axis[i]=SAMPLE_TIME*i;
            axis[i+FRAME_SIZE]=i*SAMPLE_RATE/FRAME_SIZE;
        }
        init=1;
    }
    Plotter**plt=(Plotter**)userdata;
    plt[0]->Plot2D(axis,(Real*)input,framecount);

    /* This call ruins the input */
    FFT_R2Mod((Real*)input,buf,framecount);
    plt[1]->Plot2D(axis+framecount,(Real*)buf,framecount/2);
    return paContinue;
}

int main()
{
    Plotter *plt[2]={new Plotter(0,SAMPLE_TIME*FRAME_SIZE,-1,1),new Plotter(0,SAMPLE_RATE/2,0,40)};

    plt[0]->SetTitle("Waveform");
    plt[0]->SetXText("Sample");
    plt[0]->SetYText("Amplitude");

    plt[1]->SetTitle("Spectrum");
    plt[1]->SetXText("Frequency (Hz)");

    Init_Portaudio();
    Init_Portaudio_Record(RecCallback,plt);

    while(1)
        Pa_Sleep(5000);
    Cleanup_Portaudio();

    return 0;
}
