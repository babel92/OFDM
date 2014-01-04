#include "blocks/JPlotSink.h"
#include "blocks/SignalGenerator.h"
#include "blocks/AudioSink.h"
#include "blocks/Convolution.h"
#include "blocks/Delay.h"

using namespace jsdsp;

int main()
{
	SignalGenerator sig(ST_SQRW, 400, FRAME_SIZE, SAMPLE_RATE);
	SignalGenerator sig2(ST_SINE, 400, FRAME_SIZE, SAMPLE_RATE);
	Convolution conv;
	Delay d(50);
	JPlotSink wave(0, 2048, -500, 500, "Shit");
	//AudioSink spkr;
	Connect(sig, 0, conv, 0);
	Connect(sig2, 0, conv, 1);
	Connect(conv, 0, d, 0);
	Connect(d, 0, wave, 0);

	BaseBlock::Run();
	return 0;
}