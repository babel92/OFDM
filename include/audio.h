#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include "global.h"
#include <portaudio/portaudio.h>

typedef int (*DataCallback)(const void*input,void*output,int framecount,PaTime timespan,void*userdata);

void Init_Portaudio();

void Init_Portaudio_Record(DataCallback,void*userdata);
void Init_Portaudio_Play(DataCallback, void*userdata);

void Audio_Read(void*Output, int Size = FRAME_SIZE);
void Audio_Write(void*Input, int Size = FRAME_SIZE);

void Cleanup_Portaudio();

void Portaudio_StartRecording();

void FFT_R2Mod(Real*input,Real*output,int size);

#endif // AUDIO_H_INCLUDED
