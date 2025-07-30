#pragma once
#include <type_traits>

namespace cue {

template <typename To, typename From> requires (!std::is_same_v<To, From>)
To into(const From& from);

template <typename T>
inline T into(const T& from) {
    return from;
}

// set the scale for `node` in a way that `node->getScaledContentSize()` will match `target->getScaledContentSize()`
void rescaleToMatch(cocos2d::CCNode* node, cocos2d::CCNode* target, bool stretch = false);

// set the scale for `node` in a way that `node->getScaledContentSize()` will match `targetSize`
void rescaleToMatch(cocos2d::CCNode* node, cocos2d::CCSize targetSize, bool stretch = false);

// set the scale for `node` in a way that `node->getScaledContentSize()` will match `CCSize{targetSize, targetSize}`
void rescaleToMatch(cocos2d::CCNode* node, float targetSize, bool stretch = false);

void rescaleToMatchX(cocos2d::CCNode* node, float targetWidth);
void rescaleToMatchY(cocos2d::CCNode* node, float targetHeight);

}
