#pragma once

#include <Geode/utils/cocos.hpp>

namespace cue {

enum class ListBorderStyle {
    None = 0,
    Levels = 1,
    SlimLevels = 4,

    Comments = 2,
    CommentsBlue = 3,

    Default = Levels,
};

class ListBorder : public cocos2d::CCNode {
public:
    using Type = ListBorderStyle;

    static ListBorder* create(Type type, cocos2d::CCSize size, cocos2d::ccColor4B color);

protected:
    cocos2d::ccColor4B m_bgColor;

    bool init(Type type, cocos2d::CCSize size, cocos2d::ccColor4B color);
};

}