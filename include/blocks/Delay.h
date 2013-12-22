#ifndef DELAY_H
#define DELAY_H

#include <BaseBlock.h>
#include <thread>
#include <chrono>

class Delay : public BaseBlock
{
    public:
        Delay(int millisec)
        :BaseBlock({"any in"},{"any out"})
        ,m_dura(millisec)
        {
            Ready();
        }
        virtual ~Delay() {}
    protected:
        virtual int Work(INPINS In,OUTPINS Out)
        {
			DataPtr in = In[0]->GetData();
			DataPtr out = Out[0]->AllocData(in->Size());
			memcpy(*out, *in, in->Size());
            std::this_thread::sleep_for( m_dura );
            return 0;
        }
    private:
        std::chrono::milliseconds m_dura;
};

#endif // DELAY_H
