#include "blocks/JPlotSink.h"
#include "blocks/SignalGenerator.h"
#include "blocks/AudioSink.h"

using namespace jsdsp;

int main()
{
	SignalGenerator sig(ST_SAWT, 400, FRAME_SIZE, SAMPLE_RATE);
	JPlotSink wave(0, 512, -1, 1, "Shit");
	AudioSink spkr;
	Connect(sig, 0, spkr, 0);
	Connect(sig, 0, wave, 0);

	BaseBlock::Run();
	return 0;
}