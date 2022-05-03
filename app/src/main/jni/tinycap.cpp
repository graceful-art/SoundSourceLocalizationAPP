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
#include <math.h>


#include <fcntl.h>
#include <jni.h>
#include <android/log.h>

#include "asoundlib.h"
#include "main5.h"

#define pi 3.1415926

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

struct pcm *pcm[4];
char *buffer[4];
unsigned int size;
static int open(int number)
{
    unsigned int card = 3;
    unsigned int device = 0;
    unsigned int channels = 16;
    unsigned int rate = 96000;
    unsigned int bits = 16;
    unsigned int period_size = 64;
    unsigned int period_count = 2;
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

//    signal(SIGINT, sigint_handler);
    switch (number) {
        case 0:
            pcm[number] = pcm_open(3, device, PCM_IN, &config);
            break;
        case 1:
            pcm[number] = pcm_open(4, device, PCM_IN, &config);
            break;
        case 2:
            pcm[number] = pcm_open(5, device, PCM_IN, &config);
            break;
        case 3:
            pcm[number] = pcm_open(6, device, PCM_IN, &config);
            break;
        default:
            return 1;
    }
    if (!pcm[number] || !pcm_is_ready(pcm[number])) {
        LOGD("Unable to open PCM device (%s)\n", pcm_get_error(pcm[number]));
        pcm_close(pcm[number]);
        return 1;
    }

    size = pcm_frames_to_bytes(pcm[number], pcm_get_buffer_size(pcm[number]));
    buffer[number] = (char *)malloc(size);
    if (!buffer[number]) {
        LOGD("Unable to allocate %d bytes\n", size);
        free(buffer[number]);
        pcm_close(pcm[number]);
        return 1;
    }

    return 0;
}
/*
static int read(void)
{
//    if(pcm_read(pcm, buffer, size)) {
        LOGD("Error capturing sample\n");
        return 1;
//    }
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
    res = main4(buffer,size,5);
    return 0;
}
*/
static void close(int number)
{
    free(buffer[number]);
    pcm_close(pcm[number]);
}

static void imageTransform(double specData[3621], double pfEstAngles[2], int imageData[640*480])
{
    int colorMap[] = {0x800000,0x900000,0xA00000,0xB00000,0xC00000,0xD00000,0xE00000,0xF00000,
                0xFF0000,0xFF1000,0xFF2000,0xFF3000,0xFF4000,0xFF5000,0xFF6000,0xFF7000,
                0xFF8000,0xFF9000,0xFFA000,0xFFB000,0xFFC000,0xFFD000,0xFFE000,0xFFF000,
                0xFFFF00,0xF0FF10,0xE0FF20,0xD0FF30,0xC0FF40,0xB0FF50,0xA0FF60,0x90FF70,
                0x80FF80,0x70FF90,0x60FFA0,0x50FFB0,0x40FFC0,0x30FFD0,0x20FFE0,0x10FFF0,
                0x00FFFF,0x00F0FF,0x00E0FF,0x00D0FF,0x00C0FF,0x00B0FF,0x00A0FF,0x0090FF,
                0x0080FF,0x0070FF,0x0060FF,0x0050FF,0x0040FF,0x0030FF,0x0020FF,0x0010FF,
                0x0000FF,0x0000F0,0x0000E0,0x0000D0,0x0000C0,0x0000B0,0x0000A0,0x000090,0x000080};
    double collectMatrix[8] = {465,317,0,0,0,246.098403144891,-464.966890659459,0};
    double max = specData[0], min = specData[0];
    for (int i = 0; i < 3621; ++i) {
        if (specData[i] > max) max = specData[i];
        if (specData[i] < min) min = specData[i];
    }
    double threshold = (max-min)*0.9+min;
    for (int i = pfEstAngles[0] + 125; i >= 0; --i) {
        if (i+1 > 70) continue;
        for (int j = pfEstAngles[1] + 25; j >= 0; --j) {
            if (j+1 > 50) continue;
            if (specData[i+1+71*j] < threshold && specData[i+71*(j+1)] < threshold && specData[i+1+71*(j+1)] < threshold) {
                specData[i+71*j] = min;
            }
        }
    }
    for (int i = pfEstAngles[0] + 125; i <= 70; ++i) {
        if (i-1 < 0) continue;
        for (int j = pfEstAngles[1] + 25; j >= 0; --j) {
            if (j+1 > 50) continue;
            if (specData[i-1+71*j] < threshold && specData[i+71*(j+1)] < threshold && specData[i-1+71*(j+1)] < threshold) {
                specData[i+71*j] = min;
            }
        }
    }
    for (int i = pfEstAngles[0] + 125; i >= 0; --i) {
        if (i+1 > 70) continue;
        for (int j = pfEstAngles[1] + 25; j <= 50; ++j) {
            if (j-1 < 0) continue;
            if (specData[i+1+71*j] < threshold && specData[i+71*(j-1)] < threshold && specData[i+1+71*(j-1)] < threshold) {
                specData[i+71*j] = min;
            }
        }
    }
    for (int i = pfEstAngles[0] + 125; i <= 70; ++i) {
        if (i-1 < 0) continue;
        for (int j = pfEstAngles[1] + 25; j <= 50; ++j) {
            if (j-1 < 0) continue;
            if (specData[i-1+71*j] < threshold && specData[i+71*(j-1)] < threshold && specData[i-1+71*(j-1)] < threshold) {
                specData[i+71*j] = min;
            }
        }
    }
    double x,y=1,z,p,q,azimuth,elevation,intensity;
    int a,b,index;
    for (int i = 0;i < 640; ++i) {
        for (int j = 0;j < 480; ++j) {
            p = i - collectMatrix[1]*y - collectMatrix[3];
            q = j - collectMatrix[5]*y - collectMatrix[7];
            x = (p-q*collectMatrix[2]/collectMatrix[6]) / (collectMatrix[0]-collectMatrix[4]*collectMatrix[2]/collectMatrix[6]);
            z = (q-p*collectMatrix[4]/collectMatrix[0]) / (collectMatrix[6]-collectMatrix[2]*collectMatrix[4]/collectMatrix[0]);
            azimuth = (atan(x/y))*180/pi-90;
            elevation = (atan(z/sqrt(x*x+y*y)))*180/pi;
            a = (int)(floor(azimuth));
            b = (int)(floor(elevation));
            if (a < -125 || a > -55 || b < -25 || b > 25) {
                imageData[i + j * 640] = 0x00;
                continue;
            }
            intensity = (a+1-azimuth)*(b+1-elevation)*specData[a+125+71*(b+25)]+(azimuth-a)*(b+1-elevation)*specData[a+1+125+71*(b+25)]
                       +(a+1-azimuth)*(elevation-b)*specData[a+125+71*(b+1+25)]+(azimuth-a)*(elevation-b)*specData[a+1+125+71*(b+1+25)];
            if(intensity >= threshold) {
                index = (int) ((max - intensity) / (max - threshold) * 64);
                imageData[i + j * 640] = (0xFF << 24) | colorMap[index];
            }
            else imageData[i + j * 640] = 0x00;
        }
    }
}

extern "C"
{
    JNIEXPORT jint JNICALL Java_com_company_ssl_AudioCapture_tinycap(JNIEnv *env, jobject obj) {
        return tinycap(3,0,16);
    }
    JNIEXPORT jint JNICALL Java_com_company_ssl_AudioCapture_open(JNIEnv *env, jobject obj, jint number) {
        return open(number);
    }
    JNIEXPORT jint JNICALL Java_com_company_ssl_AudioCapture_read(JNIEnv *env, jobject obj, jintArray specData, jint number) {

        if(pcm_read(pcm[number], buffer[number], size)) {
            LOGD("Error capturing sample\n");
            return 2;
        }

        short* buffer1 = (short*)(buffer[number]);
        int index;
        for(index = 1; index < size/2-1; ++index) {
            if(buffer1[index]>3276 && buffer1[index-1]>3276 && buffer1[index+1]>3276) {
                break;
            }
        }
        if(index >= size/2-1) return 1;

        LOGD("Number %d Capture succeed!\n",number);
        static const double micPos1[48] = {-0.002890,-0.008303,-0.012453,-0.014705,-0.014719,-0.012492,-0.008362,-0.002963,
                                            0.002890,0.008303,0.012453,0.014705,0.014719,0.012492,0.008362,0.002963,
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0.014719,0.012492,0.008362,0.002963,-0.002890,-0.008303,-0.012453,-0.014705,
                                           -0.014719,-0.012492,-0.008362,-0.002963,0.002890,0.008303,0.012453,0.014705};

        static const double micPos2[48] = {-0.006223,-0.013023,-0.017842,-0.019942,-0.019007,-0.015179,-0.009307,-0.001542,
                                            0.006223,0.013023,0.017842,0.019942,0.019007,0.015179,0.009307,0.001542,
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0.019007,0.015179,0.009307,0.001542,-0.006223,-0.013023,-0.017842,-0.019942,
                                           -0.019007,-0.015179,-0.009307,-0.001542,0.006223,0.013023,0.017842,0.019942};

        static const double micPos5[48] = {-0.021674,-0.030541,-0.03476,-0.033684,-0.027482,-0.017095,-0.004104,0.009507,
                                            0.021674,0.030541,0.03476,0.033684,0.027482,0.017095,0.004104,-0.009507,
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0.027482,0.017095,0.004104,-0.009507,-0.021674,0.030541,-0.03476,-0.033684,
                                           -0.027482,-0.017095,-0.004104,0.009507,0.021674,0.030541,0.03476,0.033684};

        static const double micPos6[48] = {-0.028622,-0.037136,-0.039999,-0.036769,-0.027943,-0.014863,0.0004830,0.01575,
                                            0.028622,0.037136,0.039999,0.036769,0.027943,0.014863,-0.0004830,-0.01575,
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0.027943,0.014863,-0.0004830,-0.01575,-0.028622,-0.037136,-0.039999,-0.036769,
                                           -0.027943,-0.014863,0.0004830,0.01575,0.028622,0.037136,0.039999,0.036769};
        static const double freqRange[2] = {20000, 48000};
        static const double fftMax = 0.5*32768;
        static double fftTest[128];
        static double ppfSpec2D[3621];
        static double pfEstAngles[2];
        switch(number)
        {
        case 0:
            main5((short*)(buffer[number]),micPos5,freqRange,fftMax,fftTest,ppfSpec2D,pfEstAngles);
            break;
        case 1:
            main5((short*)(buffer[number]),micPos6,freqRange,fftMax,fftTest,ppfSpec2D,pfEstAngles);
            break;
        }
        if(pfEstAngles[0] == 0) return 1;
        LOGD("azEst:%.2f elEst:%.2f", pfEstAngles[0], pfEstAngles[1]);
        static int imageData[640*480];
        imageTransform(ppfSpec2D,pfEstAngles,imageData);
        env->SetIntArrayRegion(specData, 0, 640*480, imageData);
        return 0;

    }
    JNIEXPORT void JNICALL Java_com_company_ssl_AudioCapture_close(JNIEnv *env, jobject obj, jint number) {
        close(number);
    }
}

