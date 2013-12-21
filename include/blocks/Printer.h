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
			char buf[40960];
            Data*input=In[0]->GetData();
			memcpy(buf, *input, input->Size());
			buf[input->Size()] = '\0';
            puts(buf);
            return 0;
        }
    private:
};

#endif // PRINTER_H
