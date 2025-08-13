#pragma once

#include <Geode/Geode.hpp>
#include "ListNode.hpp"

namespace cue {

class DropdownNode : public cocos2d::CCLayer {
public:
    using Callback = std::function<void(size_t, cocos2d::CCNode*)>;

    static DropdownNode* create(cocos2d::ccColor4B bgColor, float width, float cellHeight, float expandedHeight = 0.f);

    void addCell(CCNode* cell);
    void setCellColor(cocos2d::ccColor4B color);

    bool isExpanded();
    void setExpanded(bool expand);
    void setCallback(Callback&& cb);

private:
    ListNode* m_list;
    geode::Ref<ListCell> m_selected;
    cocos2d::CCSize m_fullSize;
    cocos2d::CCSize m_cellSize;
    size_t m_selectedIdx = 0;
    Callback m_callback;

    std::optional<cocos2d::CCPoint> m_trackedTouch;
    float m_accumulatedTouchDelta = 0.f;
    geode::Ref<ListCell> m_heldCell;

    bool init(cocos2d::ccColor4B bgColor, float width, float cellHeight, float expandedHeight);

    void selectCell(ListCell* cell);

    bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
    void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
    void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;
    void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) override;

    ListCell* cellForTouchPos(float pos);
    void unselectHeld();
};

}
