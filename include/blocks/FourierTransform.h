#ifndef FOURIERTRANSFORM_H
#define FOURIERTRANSFORM_H

#include <BaseBlock.h>
#include "fftw/fftw3.h"
#include <math.h>

namespace jsdsp{

	class FourierTransform : virtual public BaseBlock
	{
	public:
		FourierTransform(int Size = 1024) :BaseBlock({ "float in" }, { "float out" })
		{
			m_buffer = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)*Size);
			m_size = Size;
		}
		virtual ~FourierTransform() {}
	protected:
		int m_size;
		fftwf_complex* m_buffer;
		
		fftwf_plan m_plan;

		int DoFFT(float*in, float*out, int size)
		{
			m_plan = fftwf_plan_dft_r2c_1d(size, in, m_buffer, FFTW_ESTIMATE);
			fftwf_execute(m_plan);
			for (int i = 0; i < size / 2; ++i)
				out[i] = sqrt(m_buffer[i][0] * m_buffer[i][0] + m_buffer[i][1] * m_buffer[i][1]);

			fftwf_destroy_plan(m_plan);
			return size / 2;
		}

		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			DataPtr out = Out[0]->AllocData(in->Size() / 2);
			DoFFT((float*)in->Get(), (float*)out->Get(), in->Size());
			return 0;
		}
	private:
	};

}

#endif // FOURIERTRANSFORM_H
