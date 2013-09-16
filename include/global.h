#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <cstdarg>
#include <FL/Fl.H>

#define FRAME_SIZE (512*2)
#define SAMPLE_RATE (44100)
#define SAMPLE_TIME (1./SAMPLE_RATE)

typedef
#ifdef USE_DOUBLE
double
#else
float
#endif
Real;

typedef Real Sample;

struct PaData
{
    Sample left;
    Sample right;
};

struct CallContext
{
    void*proc;
    int argnum;
    uint32_t args[14];
};

extern "C" void SafeThisCallAgent(void*context);

void SafeCallAgentCLayer(void*context);

template <class myType>
void Invoke(myType proc,int argnum,...)
{
    union{
    myType proc;
    void*ptr;
    }shit;
    shit.proc=proc;
    CallContext*context=(CallContext*)malloc(sizeof(CallContext));
    context->proc=shit.ptr;
    context->argnum=argnum;
    va_list args;
    va_start(args,argnum);
    for(int i=0;i<argnum;++i)
        context->args[i]=va_arg(args,uint32_t);
    Fl::awake(SafeCallAgentCLayer,context);
}


#define Err_printf printf

#endif // GLOBAL_H_INCLUDED
