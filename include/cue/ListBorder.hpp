#pragma once

#include <Geode/Geode.hpp>

namespace cue {

enum class ListBorderStyle {
    Default,
    Comments,
    CommentsBlue,
};

class ListBorder : public cocos2d::CCNode {
public:
    using Type = ListBorderStyle;

    static ListBorder* create(Type type, cocos2d::CCSize size, cocos2d::ccColor4B color);

protected:
    cocos2d::ccColor4B m_bgColor;
    geode::Ref<cocos2d::extension::CCScale9Sprite> m_borderSprite;

    bool init(Type type, cocos2d::CCSize size, cocos2d::ccColor4B color);
};

}