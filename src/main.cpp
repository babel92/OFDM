#include "blocks/StringSource.h"
#include "blocks/Printer.h"
#include "blocks/AudioSource.h"
#include "blocks/PlotterSink.h"
#include "blocks/FourierTransform.h"
#include "blocks/BitGenerator.h"
#include "blocks/BitToFloat.h"
#include "blocks/Delay.h"
#include "blocks/NullSink.h"
#include <iostream>


int main()
{
    AudioSource src;
    FourierTransform fft;
    PlotterSink waveform(0,FRAME_SIZE,-1,1);
    PlotterSink spectra(0,FRAME_SIZE/2,0,40);
    Connect(src,"out",waveform,"in");
    Connect(src,"out",fft,"in");
    Connect(fft,"out",spectra,"in");
    BaseBlock::Run();
    return 0;
}
