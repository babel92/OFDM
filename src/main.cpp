#include "blocks/StringSource.h"
#include "blocks/Printer.h"
#include "blocks/AudioSource.h"
#include "blocks/PlotterSink.h"
#include "blocks/FourierTransform.h"
#include "blocks/BitGenerator.h"
#include "blocks/BitToFloat.h"
#include "blocks/Delay.h"
#include "blocks/NullSink.h"
#include "blocks/Packet.h"
#include <iostream>


int main()
{
	/*
    AudioSource src;
    FourierTransform fft;
    PlotterSink waveform(0,FRAME_SIZE,-1,1);
    PlotterSink spectra(0,FRAME_SIZE/2,0,40);
    Connect(src,"out",waveform,"in");
    Connect(src,"out",fft,"in");
    Connect(fft,"out",spectra,"in");
	*/
	StringSource src(PatternMaker("12HEAD\x5\x0\x0\x00 aawdwawd"));
	Delay delay(1000);
	PacketWrapper wrp;
	PacketExtractor ext;
	Printer prt;
	Connect(src, 0, /*delay, 0);
	Connect(delay, 0, wrp, 0);
	Connect(wrp, 0,*/ ext, 0);
	Connect(ext, 0, prt, 0);
    BaseBlock::Run();
    return 0;
}
