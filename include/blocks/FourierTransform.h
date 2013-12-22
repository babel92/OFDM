#ifndef FOURIERTRANSFORM_H
#define FOURIERTRANSFORM_H

#include <BaseBlock.h>
#include "fftw/fftw3.h"
#include <math.h>

class FourierTransform : public BaseBlock
{
    public:
        FourierTransform(int Size=1024):BaseBlock({"float in"},{"float out"})
        {
            m_buffer=(fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*Size);
            m_size=Size;
            Ready();
        }
        virtual ~FourierTransform() {}
    protected:
        int m_size;
        fftwf_complex* m_buffer;
        fftwf_plan m_plan;

        virtual int Work(INPINS In,OUTPINS Out)
        {
            DataPtr in=In[0]->GetData();
            DataPtr out=Out[0]->AllocData(in->Size());
            //memcpy(out->Get(),in->Get(),indata->Size()*2);
            //memset(out->Get(),out->Size(),0);
            m_plan=fftwf_plan_dft_r2c_1d(in->Size(),(float*)in->Get(),m_buffer,FFTW_ESTIMATE);
            fftwf_execute(m_plan);
            for(int i=0;i<in->Size()/2;++i)
                ((float*)out->Get())[i]=sqrt(m_buffer[i][0]*m_buffer[i][0]+m_buffer[i][1]*m_buffer[i][1]);
            out->Size()/=2;
            fftwf_destroy_plan(m_plan);
            return 0;
        }
    private:
};

#endif // FOURIERTRANSFORM_H
