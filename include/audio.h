#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include "portaudio/portaudio.h"

typedef int (*DataCallback)(const void*input,void*output,int framecount,PaTime timespan,void*userdata);

void Init_Portaudio();

void Init_Portaudio_Record(DataCallback,void*userdata);

void Cleanup_Portaudio();

void Portaudio_StartRecording();

#endif // AUDIO_H_INCLUDED
