#include "blocks/StringSource.h"
#include "blocks/Printer.h"
#include "blocks/AudioSource.h"
#include "blocks/FourierTransform.h"
#include "blocks/BitGenerator.h"
#include "blocks/BitToFloat.h"
#include "blocks/Delay.h"
#include "blocks/NullSink.h"
#include "blocks/Packet.h"
#include "blocks/JPlotSink.h"
#include "blocks/SpectraSink.h"
#include <iostream>

using namespace jsdsp;

int main()
{
	AudioSource src;	
	JPlotSink waveform(0, FRAME_SIZE, -1, 1, "Waveform");
	SpectraSink ss(FRAME_SIZE, SAMPLE_RATE);;
	Connect(src, "out", ss, "in");
	Connect(src, "out", waveform, "in");
	/*
	StringSource src(PatternMaker("AD1231212HEAD\x5\x0\x0\x00 aawdwdHEAD\x6\x0\x0\x0 12"));
	Delay delay(100);
	PacketWrapper wrp;
	PacketExtractor ext;
	Printer prt;
	Connect(src, 0, delay, 0);
	Connect(delay, 0, wrp, 0);
	Connect(wrp, 0, ext, 0);
	Connect(ext, 0, prt, 0);*/
    BaseBlock::Run();
    return 0;
}
