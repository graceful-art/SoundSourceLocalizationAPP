/* tinycap.c
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <time.h>


#include <fcntl.h>
#include <jni.h>
#include <android/log.h>

#include "asoundlib.h"
#include "main4.h"

#define LOGD(...) \
    __android_log_print(ANDROID_LOG_DEBUG, "AudioCaptureNative", __VA_ARGS__)

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

int capturing = 1;
int prinfo = 1;

unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            enum pcm_format format, unsigned int period_size,
                            unsigned int period_count);

void sigint_handler(int sig)
{
    capturing = 0;
}

static int tinycap(unsigned int mcard, unsigned int mdevice, unsigned int mtime)
{
    FILE *file;
    struct wav_header header;
    unsigned int card = 0;
    unsigned int device = 0;
    unsigned int channels = 16;
    unsigned int rate = 96000;
    unsigned int bits = 16;
    unsigned int frames;
    unsigned int period_size = 96;
    unsigned int period_count = 1;
    enum pcm_format format;
    int no_header = 0;

    card = mcard;
    device = mdevice;
    period_count = mtime;

    file = fopen("/data/data/com.company.ssl/record.wav", "wb");
    if (!file) {
        LOGD("Unable to create file record.wav\n");
        return 1;
    }

    header.riff_id = ID_RIFF;
    header.riff_sz = 0;
    header.riff_fmt = ID_WAVE;
    header.fmt_id = ID_FMT;
    header.fmt_sz = 16;
    header.audio_format = FORMAT_PCM;
    header.num_channels = channels;
    header.sample_rate = rate;

    switch (bits) {
    case 32:
        format = PCM_FORMAT_S32_LE;
        break;
    case 24:
        format = PCM_FORMAT_S24_LE;
        break;
    case 16:
        format = PCM_FORMAT_S16_LE;
        break;
    default:
//        fprintf(stderr, "%d bits is not supported.\n", bits);
        return 1;
    }

    header.bits_per_sample = pcm_format_to_bits(format);
    header.byte_rate = (header.bits_per_sample / 8) * channels * rate;
    header.block_align = channels * (header.bits_per_sample / 8);
    header.data_id = ID_DATA;

    /* leave enough room for header */
    if (!no_header) {
        fseek(file, sizeof(struct wav_header), SEEK_SET);
    }

    /* install signal handler and begin capturing */
    signal(SIGINT, sigint_handler);
    frames = capture_sample(file, card, device, header.num_channels,
                            header.sample_rate, format,
                            period_size, period_count);
    if (prinfo) {
        LOGD("Captured %d frames\n", frames);
    }

    /* write header now all information is known */
    if (!no_header) {
        header.data_sz = frames * header.block_align;
        header.riff_sz = header.data_sz + sizeof(header) - 8;
        fseek(file, 0, SEEK_SET);
        fwrite(&header, sizeof(struct wav_header), 1, file);
    }

    fclose(file);

    return 0;
}

unsigned int capture_sample(FILE *file, unsigned int card, unsigned int device,
                            unsigned int channels, unsigned int rate,
                            enum pcm_format format, unsigned int period_size,
                            unsigned int period_count)
{
    struct pcm_config config;
    struct pcm *pcm;
    char *buffer;
    unsigned int size;
    unsigned int bytes_read = 0;

    config.channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    config.format = format;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    pcm = pcm_open(card, device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        LOGD("Unable to open PCM device (%s)\n", pcm_get_error(pcm));
        return 0;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = (char *)malloc(size);
    if (!buffer) {
        LOGD("Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm);
        return 0;
    }

    if (prinfo) {
        LOGD("Capturing sample: %u ch, %u hz, %u bit\n", channels, rate,
           pcm_format_to_bits(format));
    }

    int count=0;
    while (count<1000 && !pcm_read(pcm, buffer, size)) {
//    if(!pcm_read(pcm, buffer, size)) {
        if (fwrite(buffer, 1, size, file) != size) {
            LOGD("Error writing file\n");
        }
        else bytes_read += size;
        count++;
//        FILE *file1;
//        file1 = fopen("/data/data/com.company.ssl/record.txt", "wb");
//        char buf[15];
//        int * res = (int*)buffer;
//        for(int i=0;i<size/4;++i) {
//            sprintf(buf,"%d\r\n",*(res+i) );
//            fwrite(buf,1,strlen(buf),file1);
//        }
//        fclose(file1);
//    }
//    else {
//        LOGD("Error capturing sample\n");
    }

    free(buffer);
    pcm_close(pcm);
    return pcm_bytes_to_frames(pcm, bytes_read);
}

struct pcm *pcm;
char *buffer;
unsigned int size;
static int open(void)
{
    unsigned int card = 3;
    unsigned int device = 0;
    unsigned int channels = 16;
    unsigned int rate = 96000;
    unsigned int bits = 16;
    unsigned int period_size = 1024;
    unsigned int period_count = 4;
    enum pcm_format format;

    switch (bits) {
        case 32:
            format = PCM_FORMAT_S32_LE;
            break;
        case 24:
            format = PCM_FORMAT_S24_LE;
            break;
        case 16:
            format = PCM_FORMAT_S16_LE;
            break;
        default:
            return 1;
    }

    struct pcm_config config;

    config.channels = channels;
    config.rate = rate;
    config.period_size = period_size;
    config.period_count = period_count;
    config.format = format;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    signal(SIGINT, sigint_handler);
    pcm = pcm_open(card, device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        LOGD("Unable to open PCM device (%s)\n", pcm_get_error(pcm));
        return 1;
    }

    size = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
    buffer = (char *)malloc(size);
    if (!buffer) {
        LOGD("Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm);
        return 1;
    }

    return 0;
}

static int read(void)
{
    if(pcm_read(pcm, buffer, size)) {
        LOGD("Error capturing sample\n");
        return 1;
    }
    FILE *file1;
    char buf[15];
    int * buffer1 = (int*)buffer;
    file1 = fopen("/data/data/com.company.ssl/record.txt", "wb");
    for(int i=0;i<size/4;++i) {
        sprintf(buf,"%d\r\n",*(buffer1+i) );
        fwrite(buf,1,strlen(buf),file1);
    }
    fclose(file1);
    LOGD("Capture succeed! %d\n",size);
    double * res;
    res = main4(buffer,size);
    return 0;
}

static void close(void)
{
    free(buffer);
    pcm_close(pcm);
}

extern "C"
{
    JNIEXPORT jint JNICALL Java_com_company_ssl_AudioCapture_tinycap(JNIEnv *env, jobject obj) {
        return tinycap(3,0,16);
    }
    JNIEXPORT jint JNICALL Java_com_company_ssl_AudioCapture_open(JNIEnv *env, jobject obj) {
        return open();
    }
    JNIEXPORT jint JNICALL Java_com_company_ssl_AudioCapture_read(JNIEnv *env, jobject obj, jdoubleArray expectedData) {
        if(pcm_read(pcm, buffer, size)) {
            LOGD("Error capturing sample\n");
            return 1;
        }
//        FILE *file1;
//        char buf[40];
//        static int count=0;
//        short * buffer1 = (short*)buffer;
//        sprintf(buf,"/data/data/com.company.ssl/record%d.txt",count);
//        file1 = fopen(buf, "wb");
//        for(int i=0;i<size/4;++i) {
//            sprintf(buf,"%d\r\n",*(buffer1+i) );
//            fwrite(buf,1,strlen(buf),file1);
//        }
//        fclose(file1);
//        count++;
        LOGD("Capture succeed! %d\n",size);
        double* res;
    clock_t start, finish;
    double duration;
    /* 测量一个事件持续的时间*/
    start = clock();
        res = main4(buffer,size);
    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    LOGD("Time:%f ",duration);
        env->SetDoubleArrayRegion(expectedData, 0, 2, res);
        return 0;
    }
    JNIEXPORT void JNICALL Java_com_company_ssl_AudioCapture_close(JNIEnv *env, jobject obj) {
        close();
    }
}

