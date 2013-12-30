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

#define NOT_USED(x) ( (void)(x) )
#define Err_printf printf

#endif // GLOBAL_H_INCLUDED
