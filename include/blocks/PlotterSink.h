#ifndef PLOTTERSINK_H
#define PLOTTERSINK_H

#include "BaseBlock.h"
#include "Plotter.h"

class PlotterSink : public BaseBlock
{
    public:
        PlotterSink(double xmin=0,double xmax=100,double ymin=0,double ymax=100)
        :BaseBlock({"float in"},{})
        ,m_plotter(xmin,xmax,ymin,ymax)
        {
            m_plotter.SetTitle("Figure");
            Ready();
        }
        virtual ~PlotterSink() {}
    protected:
        Plotter m_plotter;
        virtual int Work(INPINS In,OUTPINS Out)
        {
            Data*in=In[0]->GetData();
            m_plotter.Plot((float*)in->Get(),in->Size());
            return 0;
        }
    private:
};

#endif // PLOTTERSINK_H
