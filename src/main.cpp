#include "global.h"
#include "audio.h"
#include <iostream>
#include "plotter.h"


using namespace std;

int RecCallback(const void*input,void*output,int framecount,PaTime timespan,void*userdata)
{
    static Sample buf[FRAME_SIZE*2]={0};
    static bool init=0;
    if(!init)
    {
        for(int i=0;i<FRAME_SIZE;++i)
            buf[i]=SAMPLE_TIME*i;
        init=1;
    }
    Plotter**plt=(Plotter**)userdata;
    plt[0]->Plot2D(buf,(Real*)input,framecount);

    /* This call ruins the input */
    FFT_R2Mod((Real*)input,buf+FRAME_SIZE,framecount);
    plt[1]->Plot((Real*)(buf+FRAME_SIZE),framecount/2);
    return paContinue;
}

int main()
{
    Plotter plt1,*plt[2],plt2;
    plt[0]=&plt1;
    plt[1]=&plt2;
    plt[0]->SetTitle("Waveform");
    plt[0]->SetXMax(SAMPLE_TIME*FRAME_SIZE);
    plt[0]->SetXMin(0);
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
