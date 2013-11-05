#include "BitGenerator.h"
#include <cstring>
#include <Windows.h>

BitGenerator::BitGenerator()
:BaseBlock({},{"char out"})
{
    //ctor
}

BitGenerator::~BitGenerator()
{
    //dtor
}

int BitGenerator::Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)
{
    while(1)
    {
        Sleep(1);
        Data*ptr=(*Out)[0]->AllocData(6);
        strcpy((char*)ptr->Get(),"nimei");
        (*Out)[0]->Ready();
    }
}
