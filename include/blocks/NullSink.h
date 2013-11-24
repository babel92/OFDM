#ifndef NULLSINK_H
#define NULLSINK_H

#include <BaseBlock.h>


class NullSink : public BaseBlock
{
    public:
        NullSink():BaseBlock({"any in"},{}) {Ready();}
        virtual ~NullSink() {}
    protected:
        virtual int Work(INPINS In,OUTPINS Out)
        {
            return 0;
        }
    private:
};

#endif // NULLSINK_H
