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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "soloud.h"
#include "wavqueue.h"
#include "soloud_file.h"
// included with soloud
#include "stb_vorbis.h"

namespace SoLoud
{
    WavQueueInstance::WavQueueInstance(WavQueue *aParent)
    {
        mParent = aParent;
        mOffset = 0;
    }

    void WavQueueInstance::getAudio(float *aBuffer, unsigned int aSamples)
    {
        if (mParent->mData == NULL || mParent->mData->data == NULL)
            return;

        // Buffer size may be bigger than samples, and samples may loop..

        unsigned int written = 0;
        unsigned int maxwrite = (aSamples > mParent->mData->sampleCount) ? mParent->mData->sampleCount : aSamples;
        unsigned int channels = mChannels;

        while (written < aSamples)
        {
            unsigned int copysize = maxwrite;
            if (copysize + mOffset > mParent->mData->sampleCount)
            {
                copysize = mParent->mData->sampleCount - mOffset;
            }

            if (copysize + written > aSamples)
            {
                copysize = aSamples - written;
            }

            unsigned int i;
            for (i = 0; i < channels; i++)
            {
                memcpy(aBuffer + i * aSamples + written, mParent->mData + mOffset + i * mParent->mData->sampleCount, sizeof(float) * copysize);
            }

            written += copysize;
            mOffset += copysize;

            if (copysize != maxwrite)
            {
                if (true)
                {
                    if (mOffset == mParent->mData->sampleCount)
                    {
                        mOffset = 0;
                        mLoopCount++;
                        mParent->mData = mParent->mNext;
                        mParent->mNext = NULL;
                        (*mParent->mData->started)();
                    }
                }
                else
                {
                    for (i = 0; i < channels; i++)
                    {
                        memset(aBuffer + copysize + i * aSamples, 0, sizeof(float) * (aSamples - written));
                    }
                    mOffset += aSamples - written;
                    written = aSamples;
                }
            }
        }
    }

    bool WavQueueInstance::hasEnded()
    {
        if (mParent->mNext != NULL && mParent->mData != NULL && mOffset >= mParent->mData->sampleCount)
        {
            return 1;
        }
        return 0;
    }

    WavQueue::WavQueue()
    {
        mData = NULL;
    }

    WavQueue::~WavQueue()
    {
        stop();
    }

#define MAKEDWORD(a,b,c,d) (((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

    result WavQueue::loadwav(File *aReader)
    {
        /*int wavsize =*/ aReader->read32();
        if (aReader->read32() != MAKEDWORD('W', 'A', 'V', 'E'))
        {
            return FILE_LOAD_FAILED;
        }
        int chunk = aReader->read32();
        if (chunk == MAKEDWORD('J', 'U', 'N', 'K'))
        {
            int size = aReader->read32();
            if (size & 1)
            {
                size += 1;
            }
            int i;
            for (i = 0; i < size; i++)
                aReader->read8();
            chunk = aReader->read32();
        }
        if (chunk != MAKEDWORD('f', 'm', 't', ' '))
        {
            return FILE_LOAD_FAILED;
        }
        int subchunk1size = aReader->read32();
        int audioformat = aReader->read16();
        int channels = aReader->read16();
        int samplerate = aReader->read32();
        /*int byterate =*/ aReader->read32();
        /*int blockalign =*/ aReader->read16();
        int bitspersample = aReader->read16();

        if (audioformat != 1 ||
            subchunk1size != 16 ||
            (bitspersample != 8 && bitspersample != 16))
        {
            return FILE_LOAD_FAILED;
        }

        chunk = aReader->read32();

        if (chunk == MAKEDWORD('L', 'I', 'S', 'T'))
        {
            int size = aReader->read32();
            int i;
            for (i = 0; i < size; i++)
                aReader->read8();
            chunk = aReader->read32();
        }

        if (chunk != MAKEDWORD('d', 'a', 't', 'a'))
        {
            return FILE_LOAD_FAILED;
        }

        int readchannels = 1;

        if (channels > 1)
        {
            readchannels = 2;
            mChannels = 2;
        }

        int subchunk2size = aReader->read32();

        int samples = (subchunk2size / (bitspersample / 8)) / channels;

        mData->data = new float[samples * readchannels];

        int i, j;
        if (bitspersample == 8)
        {
            for (i = 0; i < samples; i++)
            {
                for (j = 0; j < channels; j++)
                {
                    if (j == 0)
                    {
                        mData->data[i] = ((signed)aReader->read8() - 128) / (float)0x80;
                    }
                    else
                    {
                        if (readchannels > 1 && j == 1)
                        {
                            mData->data[i + samples] = ((signed)aReader->read8() - 128) / (float)0x80;
                        }
                        else
                        {
                            aReader->read8();
                        }
                    }
                }
            }
        }
        else
            if (bitspersample == 16)
            {
                for (i = 0; i < samples; i++)
                {
                    for (j = 0; j < channels; j++)
                    {
                        if (j == 0)
                        {
                            mData->data[i] = ((signed short)aReader->read16()) / (float)0x8000;
                        }
                        else
                        {
                            if (readchannels > 1 && j == 1)
                            {
                                mData->data[i + samples] = ((signed short)aReader->read16()) / (float)0x8000;
                            }
                            else
                            {
                                aReader->read16();
                            }
                        }
                    }
                }
            }
        mBaseSamplerate = (float)samplerate;
        mData->sampleCount = samples;

        return 0;
    }

    result WavQueue::loadogg(File *aReader)
    {
        aReader->seek(0);
        MemoryFile memoryFile;
        memoryFile.openFileToMem(aReader);

        int e = 0;
        stb_vorbis *vorbis = 0;
        vorbis = stb_vorbis_open_memory(memoryFile.getMemPtr(), memoryFile.length(), &e, 0);

        if (0 == vorbis)
        {
            return FILE_LOAD_FAILED;
        }

        stb_vorbis_info info = stb_vorbis_get_info(vorbis);
        mBaseSamplerate = (float)info.sample_rate;
        int samples = stb_vorbis_stream_length_in_samples(vorbis);

        int readchannels = 1;
        if (info.channels > 1)
        {
            readchannels = 2;
            mChannels = 2;
        }
        mData->data = new float[samples * readchannels];
        mData->sampleCount = samples;
        samples = 0;
        while (1)
        {
            float **outputs;
            int n = stb_vorbis_get_frame_float(vorbis, NULL, &outputs);
            if (n == 0)
            {
                break;
            }
            if (readchannels == 1)
            {
                memcpy(mData + samples, outputs[0], sizeof(float) * n);
            }
            else
            {
                memcpy(mData + samples, outputs[0], sizeof(float) * n);
                memcpy(mData + samples + mData->sampleCount, outputs[1], sizeof(float) * n);
            }
            samples += n;
        }
        stb_vorbis_close(vorbis);

        return 0;
    }

    result WavQueue::testAndLoadFile(File *aReader)
    {
        delete[] mData;
        mData = 0;
        mData->sampleCount = 0;
        int tag = aReader->read32();
        if (tag == MAKEDWORD('O', 'g', 'g', 'S'))
        {
            return loadogg(aReader);

        }
        else if (tag == MAKEDWORD('R', 'I', 'F', 'F'))
        {
            return loadwav(aReader);
        }
        return FILE_LOAD_FAILED;
    }

    result WavQueue::load(const char *aFilename, void *finished())
    {
        DiskFile dr;
        int res = dr.open(aFilename);
        if (res != SO_NO_ERROR)
        {
            return res;
        }
        return testAndLoadFile(&dr);
    }

    result WavQueue::loadMem(unsigned char *aMem, void *finished(), unsigned int aLength, bool aCopy, bool aTakeOwnership)
    {
        if (aMem == NULL || aLength == 0)
            return INVALID_PARAMETER;

        MemoryFile dr;
        dr.openMem(aMem, aLength, aCopy, aTakeOwnership);
        return testAndLoadFile(&dr);
    }

    result WavQueue::loadFile(File *aFile, void *finished())
    {
        return testAndLoadFile(aFile);
    }

    AudioSourceInstance *WavQueue::createInstance()
    {
        return new WavQueueInstance(this);
    }
};