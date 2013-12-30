#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <BaseBlock.h>
#include "audio.h"
#include <iostream>

using namespace std;

namespace jsdsp{

	class AudioSource : public BaseBlock
	{
	public:
		AudioSource() :BaseBlock({}, { "float out" })
		{
			Init_Portaudio();
			// Blocking IO
			Init_Portaudio_Record(NULL, NULL);
		}
		virtual ~AudioSource() {}
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPinOut*outpin = Out[0];
			for (;;)
			{
				DataPtr ptr = outpin->AllocData(FRAME_SIZE);
				Audio_Read(ptr->Get());
				float* buf = (float*)ptr->Get();
				Send();
			}
			return 0;
		}
	private:
	};

}
#endif // AUDIOSOURCE_H
