#ifndef BITTOFLOAT_H
#define BITTOFLOAT_H

#include <BaseBlock.h>


class BitToFloat : public BaseBlock
{
    public:
        BitToFloat():BaseBlock({"char in"},{"float out"}) {Ready();}
    protected:
        virtual int Work(INPINS In,OUTPINS Out)
        {
            DataPtr in=In[0]->GetData();
            DataPtr out=Out[0]->AllocData(in->Size());
            for(int i=0;i<in->Size();++i)
                ((float*)out->Get())[i]=(in->Get()[i]=='1')?1.f:0.f;
            return 0;
        }
    private:
};

#endif // BITTOFLOAT_H
