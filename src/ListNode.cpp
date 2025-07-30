#include <cue/ListNode.hpp>

using namespace geode::prelude;

namespace cue {

bool ListCell::init(CCNode* inner, ListNode* list) {
    if (!CCLayerColor::initWithColor(ccColor4B{255, 255, 255, 255})) return false;

    m_inner = inner;

    float height = inner->getScaledContentHeight();
    this->setContentSize({list->getListSize().width, list->m_cellHeight == 0.f ? height : list->m_cellHeight});

    inner->ignoreAnchorPointForPosition(false);
    this->addChild(inner);

    switch (list->m_justify) {
        case Justify::Left: {
            inner->setAnchorPoint({0.0f, 0.5f});
            inner->setPosition({0.0f, this->getContentSize().height / 2.f});
        } break;
        case Justify::Center: {
            inner->setAnchorPoint({0.5f, 0.5f});
            inner->setPosition(this->getContentSize() / 2.f);
        } break;
        case Justify::Right: {
            inner->setAnchorPoint({1.0f, 0.5f});
            inner->setPosition({this->getContentSize().width, this->getContentSize().height / 2.f});
        } break;
    }

    return true;
}

bool ListNode::init(cocos2d::CCSize size, cocos2d::ccColor4B bgColor, ListBorderStyle borderStyle) {
    if (!CCLayer::init()) return false;

    m_size = size;

    this->setCellColors(
        ccColor4B{191, 114, 62, 255},
        ccColor4B{161, 88, 44, 255}
    );

    m_scrollLayer = ScrollLayer::create(size);
    this->addChild(m_scrollLayer);

    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setGap(0.f)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(size.height)
    );

    this->setContentSize(size);
    this->ignoreAnchorPointForPosition(false);

    m_border = ListBorder::create(borderStyle, size, bgColor);
    if (m_border) {
        m_border->setZOrder(1);
        m_border->setPosition(size / 2.f);
        this->addChild(m_border);
    }

    return true;
}

size_t ListNode::size() {
    return m_scrollLayer->m_contentLayer->getChildrenCount();
}

ListCell* ListNode::getCell(size_t index) {
    if (index >= this->size()) return nullptr;

    return static_cast<ListCell*>(this->getCells()->objectAtIndex(index));
}

CCArray* ListNode::getCells() {
    return m_scrollLayer->m_contentLayer->getChildren();
}

void ListNode::addCell(CCNode* cell) {
    if (cell) this->addListCell(ListCell::create(cell, this));
}

void ListNode::addListCell(ListCell* cell) {
    this->insertListCell(cell, this->size());
}

void ListNode::insertCell(CCNode* cell, size_t index) {
    if (cell) this->insertListCell(ListCell::create(cell, this), index);
}

void ListNode::insertListCell(ListCell* cell, size_t index) {
    if (!cell || index > this->size()) return;

    // move all the cells after the index one step down
    for (size_t i = index; i < this->size(); i++) {
        this->getCell(i)->setZOrder(i + 1);
    }

    cell->setColor(into<ccColor3B>(this->getCellColor(index)));
    m_scrollLayer->m_contentLayer->addChild(cell, index);

    if (m_autoUpdate) this->updateLayout();
}

void ListNode::removeCell(size_t index) {
    if (index >= this->size()) return;

    this->removeCell(this->getCell(index));
}

void ListNode::removeCell(ListCell* cell) {
    size_t idx = cell->getZOrder();

    // move all the cells after the index one step up
    for (size_t i = idx + 1; i < this->size(); i++) {
        this->getCell(i)->setZOrder(i - 1);
    }

    m_scrollLayer->m_contentLayer->removeChild(cell);
    if (m_autoUpdate) this->updateLayout();
}

void ListNode::clear() {
    m_scrollLayer->m_contentLayer->removeAllChildren();
    if (m_autoUpdate) this->updateLayout();
}

void ListNode::shuffle() {
    // todo
}

void ListNode::scrollToTop() {
    auto cl = m_scrollLayer->m_contentLayer;
    cl->setPositionY(cl->getParent()->getScaledContentHeight() - cl->getScaledContentHeight());
}

void ListNode::scrollToBottom() {
    auto cl = m_scrollLayer->m_contentLayer;
    cl->setPositionY(0);
}

void ListNode::updateLayout() {
    m_scrollLayer->m_contentLayer->updateLayout();

    // update cell colors
    for (size_t i = 0; i < this->size(); i++) {
        auto cell = this->getCell(i);
        cell->setColor(into<ccColor3B>(this->getCellColor(i)));
        cell->setZOrder(i);
    }
}

ccColor4B ListNode::getCellColor(size_t index) {
    return (index % 2 == 0) ? m_evenColor : m_oddColor;
}

ListCell* ListCell::create(cocos2d::CCNode* inner, ListNode* list) {
    auto ret = new ListCell();
    if (ret->init(inner, list)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

ListNode* ListNode::create(cocos2d::CCSize size, cocos2d::ccColor4B bgColor, ListBorderStyle borderStyle) {
    auto ret = new ListNode();
    if (ret->init(size, bgColor, borderStyle)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

}