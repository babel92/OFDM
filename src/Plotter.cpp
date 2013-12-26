#include "Plotter.h"
#include "safecall.h"
#include <iostream>

using namespace std;
static const int FPS=60;
int Plotter::m_instance=0;

std::mutex Plotter::m_mu;
std::unique_lock<std::mutex> Plotter::m_lock(m_mu);
std::condition_variable Plotter::m_cond;

void Redraw(void*canvas)
{
    Fl::add_timeout(0,Redraw,canvas);
}

void APCWrapper(void* plotter);

void timer_cb(void *v) {
   Fl::repeat_timeout(double(1.0)/FPS,timer_cb);
}

void PlotterThread(void*plotter)
{
    Fl::lock();
    Plotter *ptr=(Plotter*)plotter;
    APCWrapper((void*)plotter);

    Fl::add_timeout(0, timer_cb);
    cout<<"Plotter thread finished initialization\n";
/*
    while(Fl::wait()>0)
    {
    }*/
    Fl::run();
    cout<<"Plotter thread quit\n";
}

void APCWrapper(void* plotter)
{
    Plotter *ptr=(Plotter*)plotter;

    ptr->m_window = new Fl_Double_Window(580, 390, "Plotter");
    ptr->m_window->size_range(450,250);
    ptr->m_window->callback([](Fl_Widget*w,void*arg) { w->hide();delete (Plotter*)arg; },(void*)ptr);

    ptr->m_group =new Fl_Group(0, 0, 580, 390 );
    ptr->m_group->box(FL_DOWN_BOX);
    ptr->m_group->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);

    ptr->m_canvas = new Ca_Canvas(50, 30, 480, 290, NULL);
    ptr->m_canvas->box(FL_DOWN_BOX);
    ptr->m_canvas->color(7);
    ptr->m_canvas->align(FL_ALIGN_TOP);
    Fl_Group::current()->resizable(ptr->m_canvas);
    ptr->m_canvas->border(15);

    ptr->m_x = new Ca_X_Axis(140, 335, 300, 30, "X");
    ptr->m_x->labelsize(14);
    ptr->m_x->align(FL_ALIGN_BOTTOM);
    ptr->m_x->scale(CA_LIN);
    ptr->m_x->minimum(ptr->m_xmin);
    ptr->m_x->maximum(ptr->m_xmax);
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
    ptr->m_y->minimum(ptr->m_ymin);
    ptr->m_y->maximum(ptr->m_ymax);
    ptr->m_y->minor_grid_style(FL_DASH);
    ptr->m_y->axis_align(CA_LEFT);
    ptr->m_y->axis_color(FL_BLACK);

    ptr->m_y->current();
    ptr->m_group->end();

    Fl_Group::current()->resizable(ptr->m_group);

    ptr->m_window->end();
    ptr->m_window->show();
		
    // Inform the ctor to return

    {
        std::lock_guard<std::mutex> lock(ptr->m_mu);
        cout<<"Main thread notifying constructor\n";
        ptr->m_cond.notify_one();
    }


}

std::thread* Plotter::m_thread;
std::thread::id Plotter::m_tid;

Plotter::Plotter(double xmin,double xmax,double ymin,double ymax)
    :m_xmin(xmin),m_ymin(ymin),m_xmax(xmax),m_ymax(ymax)
{
    //ctor
    if(!m_thread)
    {
        m_thread=new std::thread(PlotterThread,this);
        m_tid=m_thread->get_id();
    }
    else
        Invoke(WRAPCALL(APCWrapper,this));
    cout<<"Plotter waiting for main thread\n";
    m_cond.wait(m_lock);
    cout<<"Main thread signaled\n";
    std::this_thread::sleep_for(chrono::milliseconds(100));
    m_instance++;
}

void Plotter::Plot(Real*buf,int size)
{
    // Duplicate memory to ensure safety, we might use Ca_LinePoint
	/*
    if(InvokeRequired())
	{
        Real*newbuf=new Real[size];
        memcpy(newbuf,buf,size*sizeof(Real));
        Invoke(WRAPCALL(&Plotter::Plot,this,buf,size));
        return;
    }*/
    Fl::lock();
    Ca_LinePoint* lp=NULL;
    Ca_Canvas::current(m_canvas);
    m_y->current();
    m_canvas->clear();
    for(int i=0;i<size;++i)
        lp=new Ca_LinePoint(lp,i,buf[i],0,FL_BLUE);
    m_canvas->redraw();
    Fl::unlock();
    Fl::awake();
    //delete[] buf;
}

void Plotter::Plot2D(Real*data1,Real*data2,int size)
{
    GUARD(Plotter::Plot2D,this,data1,data2,size);

    Ca_LinePoint* lp=NULL;
    Ca_Canvas::current(m_canvas);
    m_y->current();
    m_canvas->clear();
    for(int i=0;i<size;++i)
        lp=new Ca_LinePoint(lp,data1[i],data2[i],0,FL_BLUE);
    m_canvas->redraw();
}

Plotter::~Plotter()
{
    m_instance--;
    if(!m_instance)
        exit(0);
    //dtor
}
