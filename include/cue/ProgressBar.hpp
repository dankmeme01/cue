#pragma once

#include <Geode/Geode.hpp>

namespace cue {

class ProgressBar : public cocos2d::CCMenu {
public:
    static ProgressBar* create(
        const char* start,
        const char* middle,
        const char* end,
        const char* fill
    );

    void setContentSize(const cocos2d::CCSize& size) override;

    /// Returns the progress value in range [0; 1]
    double getValue();

    /// Sets the slider value, will be clamped if not inside the range [0; 1]
    void setValue(double value);

protected:
    double m_value = 0.0;

    cocos2d::CCSprite
        *m_outlineStart = nullptr,
        *m_outlineMiddle = nullptr,
        *m_outlineEnd = nullptr;

    cocos2d::CCSprite* m_fill = nullptr;

    bool init(
        const char* start,
        const char* middle,
        const char* end,
        const char* fill
    );

    void setup(cocos2d::CCSize size);
};

}