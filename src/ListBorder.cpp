#include <Geode/utils/terminate.hpp>
#include <cue/ListBorder.hpp>

using namespace geode::prelude;

namespace cue {

bool ListBorder::init(Type type, cocos2d::CCSize size, cocos2d::ccColor4B color) {
    if (!CCNode::init()) return false;

    m_bgColor = color;

    const char* top = "GJ_commentTop_001.png";
    const char* side = "GJ_commentSide_001.png";

    switch (type) {
        case Type::CommentsBlue: {
            top = "GJ_commentTop2_001.png";
            side = "GJ_commentSide2_001.png";
            [[fallthrough]];
        }

        case Type::Comments: {
            auto border = ListBorders::create();
            border->setSpriteFrames(top, side);
            border->setContentSize({size.width + 4.f, size.height + 6.f});
            border->setPosition(size / 2.f);
            this->addChild(border);
        } break;

        case Type::Default: {
            geode::utils::terminate("Border type Default is not yet implemented (cue::ListBorder)");
        } break;

        default: break;
    }

    this->setAnchorPoint({0.5f, 0.5f});
    this->setContentSize(size);

    return true;
}

ListBorder* ListBorder::create(Type type, cocos2d::CCSize size, cocos2d::ccColor4B color) {
    auto ret = new ListBorder();
    if (ret->init(type, size, color)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

}