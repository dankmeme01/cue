#include <cue/Util.hpp>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace cue {

template<> ccColor3B into(const ccColor4B& value) {
    return ccColor3B{value.r, value.g, value.b};
}

template<> ccColor4B into(const ccColor3B& value) {
    return ccColor4B{value.r, value.g, value.b, 255};
}

void rescaleToMatch(cocos2d::CCNode* node, cocos2d::CCNode* target, bool stretch) {
    auto targetSize = target->getScaledContentSize();
    rescaleToMatch(node, targetSize, stretch);
}

void rescaleToMatch(cocos2d::CCNode* node, cocos2d::CCSize targetSize, bool stretch) {
    auto nodeSize = node->getContentSize();

    if (!stretch) {
        float scale = targetSize.width / nodeSize.width;
        node->setScale(scale);
    } else {
        node->setScaleX(targetSize.width / nodeSize.width);
        node->setScaleY(targetSize.height / nodeSize.height);
    }
}

void rescaleToMatch(cocos2d::CCNode* node, float targetSize, bool stretch) {
    rescaleToMatch(node, {targetSize, targetSize}, stretch);
}

void rescaleToMatchX(cocos2d::CCNode* node, float targetWidth) {
    node->setScaleX(targetWidth / node->getContentWidth());
}

void rescaleToMatchY(cocos2d::CCNode* node, float targetHeight) {
    node->setScaleY(targetHeight / node->getContentHeight());
}

NineSlice* attachBackground(
    CCNode* node,
    const BackgroundOptions& options
) {
    if (!node) return nullptr;

    CCSize rawSize = node->getContentSize();
    rawSize.width += options.sidePadding * 2;
    rawSize.height += options.verticalPadding * 2;
    auto spr = createBackground(rawSize, options);

    spr->setPosition(node->getContentSize() / 2.f);
    node->addChild(spr);

    return spr;
}

NineSlice* createBackground(
    CCSize size,
    const BackgroundOptions& options
) {
    // now, we have some annoying math to figure out.
    // at 1x scale, if `rawSize` is small enough, the parts of the ccscale9sprite will overlap with each other, which is very unwanted.
    // on top of that, the `cornerRoundness` parameter will also impact the scale of the sprite,
    // as scaling the texture down and blowing up the size makes the corners a lot more rough, and vice versa.

    // here goes nothing

    float scaleX = 1.f, scaleY = 1.f;
    float roundness = options.cornerRoundness;

    auto spr = NineSlice::create(options.texture);
    auto bgSize = spr->getContentSize();

    if (size.width < bgSize.width) {
        scaleX = size.width / bgSize.width;
    }

    if (size.height < bgSize.height) {
        scaleY = size.height / bgSize.height;
    }

    if (options.scaleMustMatch) {
        scaleX = scaleY = std::min(scaleX, scaleY);
    }

    float roundnessMult = std::powf(1.8f, roundness);
    scaleX *= roundnessMult;
    scaleY *= roundnessMult;

    spr->setContentSize({size.width / scaleX, size.height / scaleY});
    spr->setScaleX(scaleX);
    spr->setScaleY(scaleY);
    spr->setOpacity(options.opacity);
    spr->setZOrder(options.zOrder);
    spr->setID(options.id);

    return spr;
}

}
