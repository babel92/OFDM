#include "Printer.h"
#include <iostream>

Printer::Printer(string prefix)
:BaseBlock({"char input"},{})
,m_prefix(prefix)
{
    //ctor
}

Printer::~Printer()
{
    //dtor
}

int Printer::Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)
{
    Data*input=(*In)[0]->GetData();
    cout<<m_prefix<<input->Get()<<endl;;
}
