#include <Geode/utils/string.hpp> // awesome geode fix
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

        case Type::Levels:
        case Type::SlimLevels: {
            bool slim = type == Type::SlimLevels;

            auto top = CCSprite::createWithSpriteFrameName(
                slim ? "GJ_table_top02_001.png" : "GJ_table_top_001.png"
            );
            auto bottom = CCSprite::createWithSpriteFrameName("GJ_table_bottom_001.png");
            auto left = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
            auto right = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");

            this->addChild(top);
            this->addChild(bottom);
            this->addChild(left);
            this->addChild(right);

            // right okay so for some reason they all have a bunch of dead space inside of them
            // so instead of nice and beautiful implementation we have to add random ass padding
            // this will definitely not play good with texture packs

            top->setAnchorPoint({0.5f, 0.f});
            top->setPosition({size.width / 2.f, size.height - 11.f});

            bottom->setAnchorPoint({0.5f, 1.f});
            bottom->setPosition({size.width / 2.f, 0.f + 11.f});

            left->setAnchorPoint({1.f, 0.5f});
            left->setPosition({0.f + 9.f, size.height / 2.f});
            left->setScaleY(size.height / left->getContentHeight());

            right->setAnchorPoint({0.f, 0.5f});
            right->setPosition({size.width - 9.f, size.height / 2.f});
            right->setScaleY(size.height / right->getContentHeight());
            right->setFlipX(true);
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