#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <cstdio>
#include <cstdlib>

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

#define Err_printf printf

#endif // GLOBAL_H_INCLUDED
