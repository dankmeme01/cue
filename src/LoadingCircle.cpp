#include <cue/LoadingCircle.hpp>

#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

namespace cue {

bool LoadingCircle::init(bool spinByDefault) {
    if (!CCSprite::initWithFile("loadingCircle.png")) {
        return false;
    }

    this->setAnchorPoint({0.5f, 0.5f});
    this->runAction(CCRepeatForever::create(
        CCRotateBy::create(1.0f, 360.f)
    ));

    this->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

    // if disabled, hide the circle
    if (!spinByDefault) {
        this->setOpacity(0);
        this->setVisible(false);
        this->onExit();
    }

    return true;
}

void LoadingCircle::addToLayer(cocos2d::CCNode* layer, int zOrder) {
    // retain in case we remove it from the parent, which would otherwise delete it
    Ref<CCNode> _self{this};

    if (this->getParent()) {
        this->removeFromParent();
    }

    layer->addChild(this, zOrder);
    this->setPosition(layer->getContentSize() / 2);
}

void LoadingCircle::fadeOut() {
    this->stopActionByTag(0);
    auto action = CCSequence::create(
        CCFadeTo::create(0.25f, 0),
        CCCallFunc::create(this, callfunc_selector(LoadingCircle::deactivate)),
        nullptr
    );
    action->setTag(0);
    this->runAction(action);
}

void LoadingCircle::deactivate() {
    this->setVisible(false);
    this->onExit();
}

void LoadingCircle::fadeIn() {
    if (this->isVisible()) return;

    this->setVisible(true);
    this->onEnter();
    this->setOpacity(0);
    auto action = CCFadeTo::create(0.25f, 255);
    action->setTag(0);
    this->runAction(action);
}

LoadingCircle* LoadingCircle::create(bool spinByDefault) {
    auto ret = new LoadingCircle;
    if (ret->init(spinByDefault)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}


}
