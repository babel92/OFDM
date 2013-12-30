#ifndef JPLOTSINK
#define JPLOTSINK

#include "global.h"
#include <BaseBlock.h>
#include "../../../JPlot/JPlot/JPlot.h"
#include <iostream>

namespace jsdsp{

	class JPlotSink : virtual public BaseBlock
	{
	public:
		JPlotSink(double xmin = 0, double xmax = 100, double ymin = 0, double ymax = 100, string Name="Who Cares") :BaseBlock({ "float in" }, {})
		{ 
			M_graph = JPlot_Init();
			if (!M_graph)
			{
				std::cerr << "Failed to init JPlot\n";
				throw;
			}
			JPlot_NewPlot(M_graph, Name);
			JPlot_SetRange(M_graph, JPXRANGE, xmin, xmax);
			JPlot_SetRange(M_graph, JPYRANGE, ymin, ymax);
		}
		virtual ~JPlotSink() {}
	protected:
		JPlot M_graph;
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			float* buf = (float*)in->Get();
			for (int i = 0; i < FRAME_SIZE; ++i)
			{
				if (buf[i]>1)
					throw;
			}
			JPlot_Draw(M_graph, (float*)in->Get(), in->Size());
			return 0;
		}
	private:
	};

}

#endif