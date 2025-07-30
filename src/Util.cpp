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

}
