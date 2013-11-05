#ifndef PRINTER_H
#define PRINTER_H

#include <BaseBlock.h>


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

#endif // PRINTER_H
