#ifndef SIGGEN
#define SIGGEN

#include "BaseBlock.h"
#include <cmath>

namespace jsdsp{

	enum SignalType
	{
		ST_SINE,
		ST_SQRW,
		ST_SAWT
	};

	class SignalGenerator : public BaseBlock
	{
	public:
		SignalGenerator(int Type, float Freq, int Framesize ,int SampleRate) :BaseBlock({}, { "float out" })
		{
			M_type = Type;
			M_frame = Framesize;
			M_samplerate = SampleRate;
			M_step = 1.f / SampleRate;
			M_freq = Freq;
			M_T = 1.f / M_freq;
			M_count = 0;
		}
		virtual ~SignalGenerator() {}
	protected:
		int M_type;
		int M_frame;
		int M_samplerate;
		int M_count;
		float M_step;
		float M_T;
		float M_freq;
		virtual int Work(INPINS In, OUTPINS Out)
		{
			for (;;)
			{
				DataPtr out = Out[0]->AllocData(M_frame);
				float* Ptr = (float*)out->Get();
				switch (M_type)
				{
				case ST_SINE:
					for (int i = 0; i < M_frame; ++i, ++M_count)
					{
						Ptr[i] = sin(3.1415927f * 2 * M_freq * M_count*M_step);
					}
					break;
				case ST_SQRW:
#define SQRW(t) (fmod(t/M_T,1)<0.5f?0:1)
					for (int i = 0; i < M_frame; ++i, ++M_count)
					{
						Ptr[i] = SQRW(M_count*M_step);
					}
					break;				
				case ST_SAWT:
#define SAWT(t) (2*fabs(fmod(t/M_T,1)-0.5f))
						for (int i = 0; i < M_frame; ++i, ++M_count)
						{
							Ptr[i] = SAWT(M_count*M_step);
						}
						break;
				default:
					break;
				}

				Send();
			}
		}
	private:

	};

}

#endif