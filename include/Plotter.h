#ifndef PLOTTER_H
#define PLOTTER_H

#include <queue>
#include "global.h"

using std::queue;

struct DataPacket
{
    Sample *data;
    int size;
};

class Plotter
{
    friend void PlotFunc();
    public:
        Plotter();
        void SetBGColor(float R,float G,float B);
        void Plot(Real*buf,int size);
        static Plotter* GetInstance(){return m_inst;}
        virtual ~Plotter();
    protected:
    private:
        static Plotter*m_inst;

        queue<DataPacket> m_pending;
};

#endif // PLOTTER_H
