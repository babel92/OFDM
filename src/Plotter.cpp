#include "Plotter.h"

#include <windows.h>
#include <process.h>
#include <cmath>
#include "safecall.h"


using namespace std;

int Plotter::m_instance=0;

void Redraw(void*canvas)
{
    Fl::add_timeout(0,Redraw,canvas);
}

void APCWrapper(void* plotter);

void PlotterThread(void*plotter)
{
    Fl::lock();
    Plotter *ptr=(Plotter*)plotter;
    APCWrapper((void*)plotter);

    // Inform the ctor to return
    ptr->m_alerter.Pulse();

    // This seems necessary to make plotting faster
    Fl::add_timeout(0,Redraw,ptr->m_canvas);

    while(Fl::wait()>0)
    {

    }
}

void APCWrapper(void* plotter)
{

    Plotter *ptr=(Plotter*)plotter;

    ptr->m_window = new Fl_Double_Window(580, 390, "Plotter");
    ptr->m_window->size_range(450,250);


    ptr->m_group =new Fl_Group(0, 0, 580, 390 );

    ptr->m_group->box(FL_DOWN_BOX);
    ptr->m_group->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);

    ptr->m_canvas = new Ca_Canvas(50, 30, 480, 290, NULL);
    ptr->m_canvas->box(FL_DOWN_BOX);
    ptr->m_canvas->color(7);
    ptr->m_canvas->align(FL_ALIGN_TOP);
    Fl_Group::current()->resizable(ptr->m_canvas);
    // w->resizable(canvas);
    ptr->m_canvas->border(15);

    ptr->m_x = new Ca_X_Axis(140, 335, 300, 30, "X");
    ptr->m_x->labelsize(14);
    ptr->m_x->align(FL_ALIGN_BOTTOM);
    ptr->m_x->scale(CA_LIN);
    ptr->m_x->minimum(0);
    ptr->m_x->maximum(100);
    ptr->m_x->label_format("%g");
    ptr->m_x->minor_grid_color(fl_gray_ramp(20));
    ptr->m_x->major_grid_color(fl_gray_ramp(15));
    ptr->m_x->label_grid_color(fl_gray_ramp(10));
    ptr->m_x->grid_visible(CA_MINOR_GRID|CA_MAJOR_GRID|CA_LABEL_GRID);
    ptr->m_x->major_step(10);
    ptr->m_x->label_step(10);
    ptr->m_x->axis_color(FL_BLACK);
    ptr->m_x->axis_align(CA_BOTTOM|CA_LINE);

    ptr->m_y = new Ca_Y_Axis(5, 30, 43, 235 /*, "I [mA]" */);

    ptr->m_y->label("Y");
    ptr->m_y->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);

    //ptr->m_yalign(FL_ALIGN_TOP_RIGHT);
    ptr->m_y->minimum(0);
    ptr->m_y->maximum(100);
    ptr->m_y->minor_grid_style(FL_DASH);
    ptr->m_y->axis_align(CA_LEFT);
    ptr->m_y->axis_color(FL_BLACK);

    ptr->m_y->current();

    new Ca_Bar(7000, 12000, 0.0011, 0.9, FL_RED,  FL_BLACK, 4,  "Bar", FL_ALIGN_TOP, FL_HELVETICA);
    new Ca_Bar(5000, 10000, 0.0011, 0.4, FL_GREEN,  FL_BLACK, 4,  "Sec.\nbar", FL_ALIGN_TOP|FL_ALIGN_INSIDE, FL_HELVETICA);



    ptr->m_group->end();

    Fl_Group::current()->resizable(ptr->m_group);
    ptr->m_window->end();
    ptr->m_window->show();
}

HANDLE Plotter::m_thread;
DWORD Plotter::m_tid;

extern "C" DWORD WINAPI GetThreadId(
    HANDLE Thread
);

Plotter::Plotter()
    :m_alerter()
{
    //ctor
    if(!m_thread)
    {
        m_thread=(HANDLE)_beginthread(PlotterThread,0,this);
        m_tid=GetThreadId(m_thread);
        m_alerter.WaitForThis();
    }
    else
        Fl::awake((Fl_Awake_Handler)APCWrapper,(void*)this);
    m_instance++;
}

void Plotter::Plot(Real*buf,int size)
{
    GUARD(&Plotter::Plot,3,this,buf,size);

    Ca_LinePoint* lp=NULL;
    Fl::lock();
    Ca_Canvas::current(m_canvas);
    m_y->current();
    m_canvas->clear();
    for(int i=0;i<size;++i)
        lp=new Ca_LinePoint(lp,i,buf[i],0,FL_BLUE);
    m_canvas->redraw();
    Fl::unlock();
}

void Plotter::Plot2D(Real*data1,Real*data2,int size)
{
    GUARD(&Plotter::Plot2D,4,this,data1,data2,size);

    Ca_LinePoint* lp=NULL;
    Fl::lock();
    Ca_Canvas::current(m_canvas);
    m_y->current();
    m_canvas->clear();
    for(int i=0;i<size;++i)
        lp=new Ca_LinePoint(lp,data1[i],data2[i],0,FL_BLUE);
    m_canvas->redraw();
    Fl::unlock();
}

Plotter::~Plotter()
{
    //dtor
}
