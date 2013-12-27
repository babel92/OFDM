#ifndef SPECTRASINK
#define SPECTRASINK

#include "FourierTransform.h"
#include "JPlotSink.h"

namespace jsdsp
{
	class SpectraSink : virtual public FourierTransform, virtual public JPlotSink
	{
	public:
		SpectraSink(int FrameSize, float SampleRate)
			:BaseBlock({ "float in" }, {}), JPlotSink(0, SampleRate / 2, 0, 40, "Spectra")
		{
			int FFTSize = FrameSize;
			M_XARR = new float[FFTSize];
			float C = SampleRate / FFTSize;
			for (int i = 0; i < FFTSize; ++i)
				M_XARR[i] = i * C;
		}
		virtual ~SpectraSink()
		{
			delete[] M_XARR;
		}
	protected:
		float* M_XARR;
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			float Transform[4096];
			int FFTSize = DoFFT((float*)in->Get(), Transform, in->Size());
			JPlot_Draw2(M_graph, M_XARR, Transform, FFTSize);
			return 0;
		}
	};
}

#endif