#ifndef JPLOTSINK
#define JPLOTSINK

#include <BaseBlock.h>
#include "../../../JPlot/JPlot/JPlot.h"
#include <iostream>

namespace jsdsp{

	class JPlotSink : virtual public BaseBlock
	{
	public:
		JPlotSink(double xmin = 0, double xmax = 100, double ymin = 0, double ymax = 100, string Name="Who Cares") :BaseBlock({ "float in" }, {})
		{ 
			if (!JPlot_Init())
			{
				std::cerr << "Failed to init JPlot\n";
				throw;
			}
			M_graph = JPlot_NewPlot(Name);
			JPlot_SetRange(M_graph, JPXRANGE, xmin, xmax);
			JPlot_SetRange(M_graph, JPYRANGE, ymin, ymax);
		}
		virtual ~JPlotSink() {}
	protected:
		JGraph M_graph;
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			JPlot_Draw(M_graph, (float*)in->Get(), in->Size());
			return 0;
		}
	private:
	};

}

#endif