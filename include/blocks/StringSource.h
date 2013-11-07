#ifndef STRINGSOURCE_H
#define STRINGSOURCE_H

#include <BaseBlock.h>
#include <string>
#include <cstring>

class StringSource : public BaseBlock
{
public:
    StringSource():BaseBlock({}, {"char out"})
    {
        Ready();
    }
    virtual ~StringSource() {}
protected:

    virtual int Work(INPINS In,OUTPINS Out)
    {
        int i=0;
        for(;;)
        {
            DataPinOut*out=Out[0];
            sprintf((char*)out->AllocData(100)->Get(),"%d\tShit",i++);
            Send();
        }
        return 0;
    }
private:
};

#endif // STRINGSOURCE_H
