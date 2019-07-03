/*
SoLoud audio engine
Copyright (c) 2013-2015 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#pragma once

#include "soloud.h"

struct stb_vorbis;

typedef struct sample {
    float * data;
    unsigned int sampleCount;
    void (*started)();
} sample;

namespace SoLoud
{
    class WavQueue;
    class File;

    class WavQueueInstance : public AudioSourceInstance
    {
        WavQueue *mParent;
        unsigned int mOffset;
    public:
        WavQueueInstance(WavQueue *aParent);
        virtual void getAudio(float *aBuffer, unsigned int aSamples);
        virtual bool hasEnded();
    };

    class WavQueue : public AudioSource
    {
        result loadwav(File *aReader);
        result loadogg(File *aReader);
        result testAndLoadFile(File *aReader);
    public:
        sample *mData; // points to the CURRENT playing sample.
        sample *mNext; // points to the NEXT sample.
        

        WavQueue();
        virtual ~WavQueue();
        /*
            Function pointer should load le next boi
            */
        result load(const char *aFilename, void *started(WavQueue callboi));
        result loadMem(unsigned char *aMem, void *started(WavQueue callboi), unsigned int aLength, bool aCopy = false, bool aTakeOwnership = true);
        result loadFile(File *aFile, void *started(WavQueue callboi));

        virtual AudioSourceInstance *createInstance();
    };
};