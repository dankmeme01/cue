#pragma once

#include "ProgressBar.hpp"
#include <functional>

namespace cue {

class Slider : public ProgressBar {
public:
    using Callback = std::function<void(Slider*, double value)>;

    static Slider* create(
        const char* start,
        const char* middle,
        const char* end,
        const char* fill,
        const char* thumb,
        const char* thumbSelected
    );

    void setContentSize(const cocos2d::CCSize& size) override;

    /// Sets the range of the values. These can be any values, but max must be greater than min.
    /// If not set, the range is [0; 1] and `getValue` will always return a value in that range.
    /// This only impacts the returned value, this does not change how the slider behaves.
    void setRange(double min, double max);

    void setCallback(Callback&& cb);
    void setCallback(const Callback& cb);

    double getValue();
    double getValueRaw();

    void setValue(double value);
    void setValueRaw(double value);


    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
    // void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent* event) override {}
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

    class Thumb : public cocos2d::CCNode {
    public:
        static Thumb* create(Slider* slider, const char* tex, const char* texsel);
        void setState(bool held);
        bool isHeld();

    private:
        Slider* m_slider;
        cocos2d::CCSprite *m_offSprite, *m_onSprite;

        bool init(Slider* slider, const char* tex, const char* texsel);
    };

private:
    double m_rangeStart = 0.0;
    double m_rangeEnd = 1.0;
    Thumb* m_thumb = nullptr;
    Callback callback;
    float m_thumbCorrection = 0.f;

    bool init(
        const char* start,
        const char* middle,
        const char* end,
        const char* fill,
        const char* thumb,
        const char* thumbSelected
    );

    void setup(cocos2d::CCSize size);
};

}