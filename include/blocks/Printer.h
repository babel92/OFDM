#ifndef PRINTER_H
#define PRINTER_H

#include <BaseBlock.h>
#include <cstdio>

class Printer : public BaseBlock
{
    public:
        Printer():BaseBlock({"char in"},{}) {Ready();}
        virtual ~Printer() {}
    protected:
        virtual int Work(INPINS In,OUTPINS Out)
        {
            Data*input=In[0]->GetData();
            puts((char*)input->Get());
            return 0;
        }
    private:
};

#endif // PRINTER_H
