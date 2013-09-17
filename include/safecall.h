#ifndef SAFECALL_H_INCLUDED
#define SAFECALL_H_INCLUDED

#include "global.h"
#include <functional>

struct CallContext
{
    void*proc;
    int calltype;
    int argnum;
    uint32_t args[13];
};

extern "C" void SafeThisCallAgent(void*context);
extern "C" void SafeStdCallAgent(void*context);

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
    context->calltype=1;
    va_list args;
    va_start(args,argnum);
    for(int i=0;i<argnum;++i)
        context->args[i]=va_arg(args,uint32_t);
    Fl::awake(SafeCallAgentCLayer,context);
}

void Invoke(std::function<void()>*proc);
void Invoke(void*proc,int argnum,...);

#define WRAPCALL(...) (new std::function<void()>(std::bind(__VA_ARGS__)))

#endif // SAFECALL_H_INCLUDED
