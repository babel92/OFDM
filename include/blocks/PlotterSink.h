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
            // This really sucks, I need to make the plotter safer with condition variable
            //Sleep(10);
            m_plotter.SetTitle("LOL");
            Ready();
        }
        virtual ~PlotterSink() {}
    protected:
        Plotter m_plotter;
        virtual int Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)
        {
            Data*in=GetPin(In,0)->GetData();
            m_plotter.Plot((float*)in->Get(),in->Size());
            return 0;
        }
    private:
};

#endif // PLOTTERSINK_H
