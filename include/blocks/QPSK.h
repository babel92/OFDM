#ifndef _INCLUDE_BLOCKS_QPSK
#define _INCLUDE_BLOCKS_QPSK

#include "BaseBlock.h"
#include "Convolution.h"
#include <cmath>
#include "fftw/fftw3.h"

namespace jsdsp
{

	class QPSKBase
	{
	protected:
		static const int SYMBOL_SIZE = 128;

		int M_count;
		float M_cf;
		float M_sr;
	};

	class QPSKModC :
		public BaseBlock
	{
	public:
		QPSKModC() : BaseBlock({ "byte in" }, { "float out" })
		{
		}

	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			DataPtr out = Out[0]->AllocData(in->Size());
			char* ptr = (char*)in->Get();
			float* wptr = (float*)out->Get();
			for (int i = 0; i < in->Size(); i += 2)
			{
				wptr[i] = (ptr[i] == '1') ? 1.f : -1.f;
				wptr[i + 1] = (ptr[i + 1] == '1') ? 1.f : -1.f;
			}
			return 0;
		}
	};

	class QAModulator
		:public BaseBlock
	{
	protected:
		float M_sr;
		float M_cf;
		float M_fs;
		int M_spf;
		int M_count;
		int M_wpos;
	public:
		QAModulator(float CarrierFreq, float SampleRate, float SamplePerSymbol)
			: BaseBlock({ "float in" }, { "float out" })
		{
			M_sr = SampleRate;
			M_cf = CarrierFreq;
			//M_fs = FrameSize;
			M_spf = SamplePerSymbol;
			M_count = 0;
			M_wpos = 0;
		}

	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			int FrameSize = in->Size()*M_spf / 2;
			DataPtr out = Out[0]->AllocData(FrameSize);
			float(*ptr)[2] = (float(*)[2])in->Get();
			float* wptr = (float*)out->Get();
			for (int i = 0; i < FrameSize; i++, M_count++)
			{
				int Symbol = i / M_spf;
				wptr[i] = ptr[Symbol][0] * cos(2 * PI*M_cf / M_sr*M_count)
					+ ptr[Symbol][1] * sin(2 * PI*M_cf / M_sr*M_count);
			}
			return 0;
		}
	};


	class QPSKMod :public BaseBlock, public QPSKBase
	{
	public:
		QPSKMod(float CarrierFreq,float SampleRate) : BaseBlock({ "byte in" }, { "float out" })
		{
			M_count = 0;
			M_cf = CarrierFreq;
			M_sr = SampleRate;
		}
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in = In[0]->GetData();
			DataPtr out = Out[0]->AllocData(512);
			char* ptr = (char*)in->Get();
			float* wptr = (float*)out->Get();
			for (int i = 0; i < in->Size(); i += 2)
			{
				if (ptr[i] == '0')
				{
					if (ptr[i + 1] == '0')
					{
						for (int i = 0; i < SYMBOL_SIZE; ++i)
							wptr[i] = cos(2 * PI * M_cf*M_count++/M_sr + 1.25*PI);
					}
					else
					{
						for (int i = 0; i < SYMBOL_SIZE; ++i)
							wptr[i] = cos(2 * PI * M_cf * M_count++ / M_sr + 0.75*PI);
					}
				}
				else
				{
					if (ptr[i + 1] == '0')
					{
						for (int i = 0; i < SYMBOL_SIZE; ++i)
							wptr[i] = cos(2 * PI * M_cf * M_count++ / M_sr + 1.75*PI);
					}
					else
					{
						for (int i = 0; i < SYMBOL_SIZE; ++i)
							wptr[i] = cos(2 * PI * M_cf * M_count++ / M_sr + 0.25*PI);
					}
				}
				wptr += SYMBOL_SIZE;
			}
			return 0;
		}
	};

	void QPSKNormailize(float* Arr, int Size)
	{
		float min = Arr[0];
		float max = Arr[0];
		for (int i = 0; i < Size; ++i)
		{
			if (min>Arr[i])
				min = Arr[i];
			if (max < Arr[i])
				max = Arr[i];
		}
		float shift = -(max + min) / 2;
		float co = (max - min)/2;
		for (int i = 0; i < Size; ++i)
		{
			Arr[i] = (Arr[i] + shift) / co;
		}
	}

	// Determine binary symbols from I,Q arrays (non-null terminated)
	void QPSKDetermine(float* I, float*Q, int FrameSize, int SymbolPerFrame, char* Out)
	{
		float sumI, sumQ;
		int delta = FrameSize / SymbolPerFrame;
		int i;
		for (i = 0; i < SymbolPerFrame; ++i)
		{
			sumI = sumQ = 0;
			for (int j = i*delta; j < (i + 1)*delta; ++j)
			{
				sumI += I[j];
				sumQ += Q[j];
			}
			sumI /= SymbolPerFrame;
			sumQ /= SymbolPerFrame;
			Out[i * 2] = sumI>0 ? '1' : '0';
			Out[i * 2 + 1] = sumQ > 0 ? '1' : '0';
		}
	}

	class QPSKDemod :virtual public BaseBlock, public QPSKBase
	{
	public:
		QPSKDemod(float CarrierFreq, float SampleRate)
			:BaseBlock({ "float in" }, { "float outI", "float outQ" })
		{
			M_count = 0;
			M_cf = CarrierFreq;
			M_sr = SampleRate;
			M_cbuf1 = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)* 512 * 2);
			M_rbuf1 = new float[512 * 2];
			M_cbuf2 = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)* 512 * 2);
			M_rbuf2 = new float[512 * 2];
			M_plan1 = fftwf_plan_dft_r2c_1d(512, M_rbuf1, M_cbuf1, FFTW_ESTIMATE);
			M_iplan1 = fftwf_plan_dft_c2r_1d(512, M_cbuf1, M_rbuf1, FFTW_ESTIMATE);
			M_plan2 = fftwf_plan_dft_r2c_1d(512, M_rbuf2, M_cbuf2, FFTW_ESTIMATE);
			M_iplan2 = fftwf_plan_dft_c2r_1d(512, M_cbuf2, M_rbuf2, FFTW_ESTIMATE);
		}
	protected:
		fftwf_complex* M_cbuf1;
		float* M_rbuf1;
		fftwf_complex* M_cbuf2;
		float* M_rbuf2;
		fftwf_plan M_plan1;
		fftwf_plan M_iplan1;
		fftwf_plan M_plan2;
		fftwf_plan M_iplan2;
		int M_count;
		virtual int Work(INPINS In, OUTPINS Out)
		{
			char buf[16];
			DataPtr in = In[0]->GetData();
			DataPtr outi = Out[0]->AllocData(512);
			DataPtr outq = Out[1]->AllocData(512);
			float* IN = (float*)in->Get();
			float* I = (float*)outi->Get();
			float* Q = (float*)outq->Get();
#if 1
			for (int i = 0; i < 512; ++i, ++M_count)
			{
				I[i] = sin(2 * PI * M_cf*M_count / M_sr)*IN[i];
				Q[i] = cos(2 * PI * M_cf*M_count / M_sr)*IN[i];
			}
#else
			for (int i = 0; i < 512; ++i,++M_count)
			{
				M_rbuf1[i] = sin(2 * PI * M_cf*M_count / M_sr)*IN[i];
				M_rbuf2[i] = cos(2 * PI * M_cf*M_count / M_sr)*IN[i];
			}
			
			// LPF
			fftwf_execute(M_plan1);
			fftwf_execute(M_plan2);
			for (int i = 1.4 * M_cf / M_sr * 512; i < 512; ++i)
			{
				M_cbuf1[i][0] = M_cbuf1[i][1] = M_cbuf2[i][0] = M_cbuf2[i][1] = 0;
			}
			fftwf_execute(M_iplan1);
			fftwf_execute(M_iplan2);
			for (int i = 0; i < 512; ++i, ++M_count)
			{
				I[i] = M_rbuf1[i] / 512;
				Q[i] = M_rbuf2[i] / 512;
			}
#endif
			//QPSKNormailize(I, 512);
			//QPSKNormailize(Q, 512);
			QPSKDetermine(I, Q, 512, 4, buf);
			buf[8] = 0;
			std::cout << buf << std::endl;
			return 0;
		}
	};

}

#endif