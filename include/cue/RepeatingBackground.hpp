#pragma once
#include <Geode/Geode.hpp>

namespace cue {

enum class RepeatMode {
    X, Y, Both
};

class RepeatingBackground : public cocos2d::CCSprite {
public:
    static RepeatingBackground* create(const char* texture, float scale = 1.0f, RepeatMode mode = RepeatMode::X, cocos2d::CCSize visibleSize = {});

    inline void setSpeed(float speed) { m_speed = speed; }
    inline float getSpeed() const { return m_speed; }

protected:
    float m_speed;
    RepeatMode m_mode;
    cocos2d::CCSize m_visibleSize;

    bool init(const char* texture, float scale, RepeatMode mode, cocos2d::CCSize visibleSize);
    void update(float dt);
};

}