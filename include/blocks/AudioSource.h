#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <BaseBlock.h>
#include "audio.h"

class AudioSource : public BaseBlock
{
    public:
        AudioSource():BaseBlock({},{"float out"})
        {
            Init_Portaudio();
            // Blocking IO
            Init_Portaudio_Record(NULL,NULL);
            Ready();
        }
        virtual ~AudioSource() {}
    protected:
        virtual int Work(INPINS In,OUTPINS Out)
        {
            DataPinOut*outpin=Out[0];
            for(;;)
            {
                Data*ptr=outpin->AllocData(FRAME_SIZE);
                Audio_Read(ptr->Get());
                Send();
            }
            return 0;
        }
    private:
};

#endif // AUDIOSOURCE_H
