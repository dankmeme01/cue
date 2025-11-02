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

struct BackgroundOptions {
    uint8_t opacity = 80;

    float sidePadding = 3.f;
    float verticalPadding = 3.f;

    // 0 = square, 1 = slightly rounded (depends on texture though)
    // TODO: currently unused
    float cornerRoundness = 1.f;

    bool scaleMustMatch = true; // determines if scalex must be equal to scaley (impacts how pretty the corners are)
    const char* texture = "square02_001.png";
    int zOrder = -1;
    std::string id = "background";
};

/// Attach a CCScale9Sprite background to a node.
/// This calls `createBackground` and adds the background as a child to the given node,
/// for more information check `createBackground`.
cocos2d::extension::CCScale9Sprite* attachBackground(
    cocos2d::CCNode* node,
    const BackgroundOptions& options = BackgroundOptions{}
);

/// Creates a CCScale9Sprite background with the given options and size.
/// See the `BackgroundOptions` struct for all the options.
/// Note: `sidePadding` and `verticalPadding` settings have no effect, change the `size` parameter instead.
cocos2d::extension::CCScale9Sprite* createBackground(
    cocos2d::CCSize size,
    const BackgroundOptions& options = BackgroundOptions{}
);

// Call removeFromParent on the node and set it to null. Does nothing if null.
template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
void resetNode(T*& node) {
    if (node) {
        node->removeFromParent();
        node = nullptr;
    }
}

// Call removeFromParent on the node and set it to null. Does nothing if null.
template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
void resetNode(geode::Ref<T>& node) {
    if (node) {
        node->removeFromParent();
        node = nullptr;
    }
}

}
