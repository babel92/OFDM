#ifndef BITGENERATOR_H
#define BITGENERATOR_H

#include <BaseBlock.h>


class BitGenerator : public BaseBlock
{
    public:
        BitGenerator();
        virtual ~BitGenerator();
    protected:
        virtual int Work(vector<DataPinIn*>*In,vector<DataPinOut*>*Out);
    private:
};

#endif // BITGENERATOR_H
