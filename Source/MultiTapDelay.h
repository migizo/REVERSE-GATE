//
//  multiTapDelay.h
//  reverseGate
//
//  Created by migizo on 2021/02/02.
//

#ifndef multiTapDelay_h
#define multiTapDelay_h

#include <deque>
#include <cmath>

class MultiTapDelay {
public:

    // TODO: LPF? preDelayTimeに名前変える?
    // fadeCounter, fadeState : timeを変えた時のプチプチ音対策, 音をフェードで消す
    // fadeCountWait : 連続してtimeを変えた時にフェードインするのタイミングを遅らせる
    // time, tapSamplesの単位はms
//    MultiTapDelay(const std::vector<int> tapSamples)
//    {
//        this->tapSamples = tapSamples;
//        calculate();
//    }
    //------------------------------------------------------------------------
    // tapSamples配列を用意、一番長いtapSamplesに合わせてbufferリサイズ、0でクリア
    MultiTapDelay()
    {
        tapSamples =
        {2,   3,   5,   7,  11,  13,  17,  19,  23,  29,
        31,  37,  41,  43,  47,  53,  59,  61,  67,  71,
        73,  79,  83,  89,  97};
        calculate();
    }
    //------------------------------------------------------------------------
    template<typename SampleType>
    void process(juce::AudioBuffer<SampleType>& audioBuffer, int channel, int numSamples)
    {
        auto* inBuf = audioBuffer.getReadPointer (channel);
        auto* outBuf = audioBuffer.getWritePointer (channel);
        for (int i = 0; i < numSamples; i++) {
            
            float fadeVolume = 1.0f;
            if (fadeState != FADE_NONE)
            {
                fadeVolume = fminf(1.0f, fmaxf(0.0f, (float)fadeCounter / (float)fadeCountMax));
                if (fadeState == FADE_OUT)
                {
                    fadeCounter--;
                    if (fadeCounter <= 0 - fadeCountWait)
                    {
                        fadeState = FADE_IN;
                        fadeCountWait = 0;
                        calculate();
                    }
                }
                else if (fadeState == FADE_IN)
                {
                    fadeCounter++;
                    if (fadeCounter >= fadeCountMax + fadeCountWait) fadeState = FADE_NONE;
                }
            }
            // 現在の音を取得しpush
            SampleType tmp = inBuf[i];
            SampleType inTmp = tmp;
            if (abs(tmp) < 1E-4) tmp = 0;
            buffer.push_front(tmp);
            buffer.resize(tapSampleMaxSize);

            // 配列の、先頭からtapSamples分ずらした位置のを読み込み加算
            tmp = 0;
            for (int j = 0; j < tapTotalNum; j++) {
                tmp += buffer[calculateTapSamples[j]] * tapVolumes[j];
            }
            tmp *= fadeVolume;
            tmp = fminf(1.0, fmaxf(-1.0, tmp));

            // delay音を返す
            outBuf[i] = ((1.0f - mix) * inTmp + mix * tmp) * volume;
        }
    }
    //------------------------------------------------------------------------
    void setSampleRate(float sampleRate)
    {
        if (this->sampleRate != sampleRate) standbyCalculate();
        this->sampleRate = sampleRate;
    }
    //------------------------------------------------------------------------
    void setRoomSize(float roomSize)
    {
        this->roomSize = roomSize;
        standbyCalculate();
    }
    //------------------------------------------------------------------------
    void setDelayTime(float delayTime)
    {
        this->delayTime = delayTime;
        standbyCalculate();
    }
    //------------------------------------------------------------------------
    void setMix(float mix)
    {
        this->mix = mix / 100.0f;
    }
    //------------------------------------------------------------------------
    void setVolume(float volume)
    {
        this->volume = volume;
    }
    //------------------------------------------------------------------------
    void setRoomSizeMax(float roomSizeMax)
    {
        if (this->roomSizeMax != roomSizeMax) standbyCalculate();
        this->roomSizeMax = roomSizeMax;
    }
    //------------------------------------------------------------------------
    void setDelayTimeMax(float delayTimeMax)
    {
        if (this->delayTimeMax != delayTimeMax) standbyCalculate();
        this->delayTimeMax = delayTimeMax;
    }
    //------------------------------------------------------------------------
    void standbyCalculate()
    {
        if (fadeState != FADE_NONE) {
            fadeCountWait = FADE_OUT ? fadeCounter : (fadeCountMax - fadeCounter);
            return;
        }
        fadeCountMax = (int)(50.0f / 1000.0f * 44100.0f);
        fadeCounter = fadeCountMax; // 30~50msで補間
        fadeState = FADE_OUT;
        fadeCountWait = 0;
    }
private:

    void calculate()
    {
        tapTotalNum = tapSamples.size();
        tapVolumes.resize(tapTotalNum);
        calculateTapSamples.resize(tapTotalNum);
        for (int i = 0; i < tapTotalNum; i++) {
            tapVolumes[i] = 0.02f * (i+1);
            calculateTapSamples[i] = (int)getSampleSize(delayTime, roomSize, tapSamples[i], i);
        }
        tapSampleMaxSize = 1 + getSampleSize(delayTimeMax, roomSizeMax, 100, tapTotalNum - 1);
        if (buffer.size() > tapSampleMaxSize) {}
        else {
            std::vector<float> zeroBuf(tapSampleMaxSize - buffer.size());
            buffer.insert(buffer.end(), zeroBuf.begin(), zeroBuf.end());
        }
    }
    int getSampleSize(float delayTime, float roomSize, int tapSample, int tapID)
    {
        return (((delayTime + tapSample + roomSize * (float)tapID)/1000.0f)*sampleRate);
    }
    //------------------------------------------------------------------------
    enum FadeState
    {
        FADE_NONE = 0,
        FADE_OUT,
        FADE_IN
    } fadeState = FADE_NONE;
    int fadeCounter;
    int fadeCountMax;
    int fadeCountWait;
    
    std::deque<double> buffer;
    std::vector<int> tapSamples;
    std::vector<float> tapVolumes;
    std::vector<int> calculateTapSamples;
    int tapTotalNum;
    int tapSampleMaxSize;
    float delayTime = 15.0f;
    float roomSize = 30.0f;
    float mix = 1.0f;
    float volume = 1.0f;
    float delayTimeMax = 50.0f;
    float roomSizeMax = 500.0f;
    float sampleRate = 44100.0f;
};

#endif /* multiTapDelay_h */
