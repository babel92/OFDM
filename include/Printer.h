#ifndef PRINTER_H
#define PRINTER_H

#include <BaseBlock.h>
#include <string>
#include <cstring>

class Printer : public BaseBlock
{
    public:
        Printer(string prefix);
        virtual ~Printer();
    protected:
        virtual int Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out);
    private:
        string m_prefix;
};

class Concatenater:public BaseBlock
{
public:
    Concatenater():BaseBlock({"char in1","char in2"},{"char out"}){}
    virtual ~Concatenater(){}
protected:
    virtual int Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out)
    {
        Data*In1=GetPin(In,0)->GetData();
        Data*In2=GetPin(In,1)->GetData();
        std::string str=(char*)In1->Get();
        str+=(char*)In2->Get();
        strcpy((char*)GetPin(Out,0)->AllocData(100)->Get(),str.c_str());
    }
};

#endif // PRINTER_H
