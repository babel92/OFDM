#include "blocks/StringSource.h"
#include "blocks/Printer.h"
#include "blocks/AudioSource.h"
#include "blocks/PlotterSink.h"
#include "blocks/FourierTransform.h"
#include "blocks/BitGenerator.h"
#include "blocks/BitToFloat.h"

int main()
{
    BitGenerator src("0011");
    BitToFloat conv;
    PlotterSink waveform(0,100,0,2);

    Connect(src,"out",conv,"in");
    Connect(conv,"out",waveform,"in");

    BaseBlock::Run();
    return 0;
}
