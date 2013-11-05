#include "BitGenerator.h"
#include <iostream>

BitGenerator::BitGenerator()
:BaseBlock({},{"int out","byte a"})
{
    //ctor
}

BitGenerator::~BitGenerator()
{
    //dtor
}

int BitGenerator::Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)
{
    std::cout<<(*Out)[1]->GetName();
}
