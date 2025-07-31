#pragma once

#include <cocos2d.h>

namespace cue {

class LoadingCircle : public cocos2d::CCSprite {
public:
    // Creates the loading circle, by default is invisible unless `fadeIn` is called or `true` is passed to `create`.
    static LoadingCircle* create(bool spinByDefault = false);

    void fadeOut();
    void fadeIn();

    // Adds the loading circle to the center of the given layer.
    void addToLayer(cocos2d::CCNode* layer, int zOrder = 999);

private:
    bool init(bool spinByDefault);
    void deactivate();
};

}