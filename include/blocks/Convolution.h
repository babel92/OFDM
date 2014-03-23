#ifndef CONV
#define CONV

#include "BaseBlock.h"

namespace jsdsp
{
	class Convolution : virtual public BaseBlock
	{
	public:
		Convolution()
			:BaseBlock({ "float in1", "float in2" }, { "float out" })
		{

		}
	protected:
		int Convolve(const float* Sig1, size_t SignalLen,
			const float* Sig2, size_t KernelLen,
			float* Result)
		{
			size_t n;

			for (n = 0; n < SignalLen + KernelLen - 1; n++)
			{
				size_t kmin, kmax, k;

				Result[n] = 0;

				kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
				kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

				for (k = kmin; k <= kmax; k++)
				{
					Result[n] += Sig1[k] * Sig2[n - k];
				}
			}
			return SignalLen + KernelLen - 1;
		}

		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr in1 = In[0]->GetData();
			DataPtr in2 = In[1]->GetData();
			DataPtr out = Out[0]->AllocData(in1->Size() + in2->Size() - 1);

			Convolve((float*)in1->Get(), in1->Size(),
				(float*)in2->Get(), in2->Size(),
				(float*)out->Get());
			return 0;
		}
	};
}

#endif