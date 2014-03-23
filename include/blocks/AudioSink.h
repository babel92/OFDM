#ifndef AUDIOSINK
#define AUDIOSINK

#include <BaseBlock.h>
#include "audio.h"
#include <iostream>

using namespace std;

namespace jsdsp{

	class AudioSink : public BaseBlock
	{
	public:
		AudioSink() :BaseBlock({"float in"}, { })
		{
			Init_Portaudio();
			// Blocking IO
			Init_Portaudio_Play(NULL, NULL);
		}
		virtual ~AudioSink() {}
	protected:
		virtual int Work(INPINS In, OUTPINS Out)
		{
			DataPtr ptr = In[0]->GetData();
			Audio_Write(ptr->Get(), ptr->Size());
			return 0;
		}
	private:
	};

}

#endif