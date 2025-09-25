#include <cue/DropdownNode.hpp>

using namespace geode::prelude;

// TODO: fix scroll wheel

namespace cue {

constexpr static float DELTA_ALLOWANCE = 20.f;

bool DropdownNode::init(cocos2d::ccColor4B bgColor, float width, float cellHeight, float expandedHeight) {
    if (!CCLayer::init()) return false;

    m_fullSize = CCSize{width, expandedHeight + cellHeight};
    m_cellSize = CCSize{width, cellHeight};
    m_expandedHeight = expandedHeight;

    auto layout = ColumnLayout::create()->setGap(0.f);
    layout->ignoreInvisibleChildren(true);
    m_layoutContainer = CCNode::create();
    m_layoutContainer->setContentSize(m_fullSize);
    m_layoutContainer->setLayout(layout);
    m_layoutContainer->setAnchorPoint({0.f, 0.f});
    this->addChild(m_layoutContainer);

    m_list = ListNode::create({width, expandedHeight}, bgColor, ListBorderStyle::None);
    m_list->setCellHeight(cellHeight);
    m_list->setAnchorPoint({0.5f, 1.0f});
    m_list->setPosition(width / 2.f, expandedHeight);
    m_list->setVisible(false);
    m_list->setOverscrollEnabled(false);
    m_list->setCellColor(ccColor4B{0, 0, 0, 0});
    m_list->setZOrder(-999999999);
    m_layoutContainer->addChild(m_list);

    auto bg = CCLayerColor::create(bgColor, width, cellHeight);
    bg->setAnchorPoint({0.f, 0.f});
    bg->ignoreAnchorPointForPosition(false);
    bg->setPosition(0.f, 0.f);
    bg->setZOrder(-2);
    this->addChild(bg);
    m_cellBg = bg;

    auto arrow = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    arrow->setScale(0.8f);
    arrow->setPosition({width - 15.f, m_cellSize.height / 2.f});
    arrow->setZOrder(3);
    m_cellBg->addChild(arrow);

    this->setAnchorPoint({0.5f, 0.5f});
    this->setContentSize(m_fullSize);
    this->ignoreAnchorPointForPosition(false);
    this->setTouchEnabled(true);
    this->setTouchMode(cocos2d::kCCTouchesOneByOne);

    this->setExpanded(false);

    return true;
}

void DropdownNode::setCellColor(cocos2d::ccColor4B color) {
    m_list->setCellColor(color);
}

bool DropdownNode::isExpanded() {
    return m_list->isVisible();
}

void DropdownNode::setExpanded(bool expand) {
    m_list->setVisible(expand);

    if (expand) {

        // set the size of the list to the size of the contents
        float height = std::min(m_expandedHeight, m_list->size() * m_cellSize.height);
        m_list->setFullHeight(height);

        m_layoutContainer->setContentHeight(m_cellSize.height + height);
        this->setContentHeight(m_cellSize.height + height);
        m_cellBg->setPositionY(height);

        m_list->scrollToTop();
    } else {
        m_layoutContainer->setContentHeight(m_cellSize.height);
        this->setContentHeight(m_cellSize.height);
        m_cellBg->setPositionY(0.f);
    }

    m_layoutContainer->updateLayout();
    this->unselectHeld();
}

void DropdownNode::setCallback(Callback&& cb) {
    m_callback = std::move(cb);
}

void DropdownNode::addCell(CCNode* cell) {
    auto lc = m_list->addCell(cell);

    if (!m_selected) {
        this->selectCell(lc);
    }
}

void DropdownNode::selectCell(ListCell* cell) {
    if (m_selected) {
        m_selected->removeFromParent();
        m_list->insertListCell(m_selected, m_selectedIdx);
        m_selected = nullptr;
    }

    m_selected = cell;
    m_selected->setVisible(true);
    m_selectedIdx = m_list->indexForCell(cell);
    m_list->removeCell(cell);

    m_selected->setPosition(m_fullSize.width / 2.f, m_fullSize.height - m_cellSize.height / 2.f);
    m_layoutContainer->addChild(m_selected);
    m_layoutContainer->updateLayout();

    if (m_callback) {
        m_callback(m_selectedIdx, m_selected->getInner());
    }
}

bool DropdownNode::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    auto pos = this->convertTouchToNodeSpace(pTouch);

    if (pos.x < 0 || pos.x > m_fullSize.width || pos.y < 0 || pos.y > m_fullSize.height) {
        // touch out of bounds, close the dropdown if it's open
        if (this->isExpanded()) {
            this->setExpanded(false);
        }

        return false;
    }

    m_trackedTouch = pos;
    m_accumulatedTouchDelta = 0.f;

    if (this->isExpanded()) {
        if (auto cell = this->cellForTouchPos(pos.y)) {
            cell->runAction(CCSequence::create(
                CCFadeTo::create(0.05f, 80),
                nullptr
            ));
            m_heldCell = cell;
        }
    }

    return true;
}

void DropdownNode::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    if (!m_trackedTouch) return;

    auto pos = this->convertTouchToNodeSpace(pTouch);
    float delta = pos.getDistance(*m_trackedTouch);
    m_accumulatedTouchDelta += delta;
    m_trackedTouch = pos;

    if (m_accumulatedTouchDelta > DELTA_ALLOWANCE) {
        this->unselectHeld();
        m_trackedTouch = std::nullopt;
    }
}

void DropdownNode::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    if (!m_trackedTouch) return;

    auto pos = this->convertTouchToNodeSpace(pTouch);
    m_trackedTouch = std::nullopt;

    bool expanded = this->isExpanded();

    auto selfSize = this->getContentSize();
    if (pos.x < 0 || pos.x > selfSize.width || pos.y < 0 || pos.y > selfSize.height) return;

    if (pos.y > selfSize.height - m_cellSize.height) {
        // toggle expanded state
        this->setExpanded(!expanded);
        return;
    }

    // select the held item
    if (m_heldCell) {
        this->selectCell(m_heldCell);
        this->setExpanded(false);
        this->unselectHeld();
    }
}

ListCell* DropdownNode::cellForTouchPos(float pos) {
    size_t cellCount = m_list->size();

    // scrollAmount = 0 when at the bottom of the list, goes up as you scroll up
    float scrollAmount = cellCount * m_cellSize.height - m_list->getScrollPos().val;
    float clickPosFromBottom = scrollAmount + pos;
    size_t backwardsIdx = clickPosFromBottom / m_cellSize.height; // 0 - bottom cell
    if (backwardsIdx >= cellCount) {
        return nullptr;
    }

    size_t idx = cellCount - backwardsIdx - 1;
    return m_list->getCell(idx);
}

void DropdownNode::unselectHeld() {
    if (!m_heldCell) return;

    m_heldCell->stopAllActions();
    m_heldCell->setOpacity(0);
    m_heldCell = nullptr;
}

void DropdownNode::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent) {
    m_trackedTouch = std::nullopt;
}

DropdownNode* DropdownNode::create(cocos2d::ccColor4B bgColor, float width, float cellHeight, float expandedHeight) {
    auto ret = new DropdownNode;
    if (ret->init(bgColor, width, cellHeight, expandedHeight)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

}