#include <cue/ListNode.hpp>

using namespace geode::prelude;

namespace cue {

bool ListCell::init(CCNode* inner, ListNode* list) {
    float height = inner->getScaledContentHeight();
    CCSize selfSize{list->getListSize().width, list->m_cellHeight == 0.f ? height : list->m_cellHeight};

    if (!CCLayerColor::initWithColor(ccColor4B{255, 255, 255, 255}, selfSize.width, selfSize.height)) return false;

    m_inner = inner;

    this->setContentSize(selfSize);
    this->setCascadeColorEnabled(false);
    this->setCascadeOpacityEnabled(false);

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

void ListCell::draw() {
    CCLayerColor::draw();

    // i stole this from geode
    auto size = this->getContentSize();
    cocos2d::ccDrawColor4B(0, 0, 0, 0x4f);
    glLineWidth(2.0f);
    cocos2d::ccDrawLine({ 1.0f, 0.0f }, { size.width - 1.0f, 0.0f });
    cocos2d::ccDrawLine({ 1.0f, size.height }, { size.width - 1.0f, size.height });
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

    auto bg = CCLayerColor::create(bgColor, size.width, size.height);
    bg->setAnchorPoint({0.5f, 0.5f});
    bg->setPosition(size / 2.f);
    bg->ignoreAnchorPointForPosition(false);
    bg->setZOrder(-2);
    this->addChild(bg);

    this->scheduleUpdate();

    return true;
}

void ListNode::update(float dt) {
    if (!m_overscroll) {
        auto cl = m_scrollLayer->m_contentLayer;
        float ypos = cl->getPositionY();
        auto cont = this->contentSize();

        if (ypos > 0.f) {
            cl->setPositionY(0.f);
        } else if (ypos < -cont) {
            cl->setPositionY(-cont);
        }
    }
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

ListCell* ListNode::addCell(CCNode* cell) {
    if (!cell) return nullptr;

    auto lc = ListCell::create(cell, this);
    this->addListCell(lc);
    return lc;
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

    auto col = this->getCellColor(index);
    cell->setColor(into<ccColor3B>(col));
    cell->setOpacity(col.a);

    m_scrollLayer->m_contentLayer->addChild(cell, index);

    if (m_autoUpdate) this->updateLayout();
}

void ListNode::removeCell(size_t index) {
    if (index >= this->size()) return;

    this->removeCell(this->getCell(index));
}

void ListNode::removeCell(ListCell* cell) {
    size_t idx = this->indexForCell(cell);

    // move all the cells after the index one step up
    for (size_t i = idx + 1; i < this->size(); i++) {
        this->getCell(i)->setZOrder(i - 1);
    }

    m_scrollLayer->m_contentLayer->removeChild(cell);
    if (m_autoUpdate) this->updateLayout();
}

size_t ListNode::indexForCell(ListCell* cell) {
    return cell->getZOrder();
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
    cl->setPositionY(-this->contentSize());
}

float ListNode::contentSize() {
    auto cl = m_scrollLayer->m_contentLayer;
    return - (cl->getParent()->getScaledContentHeight() - cl->getScaledContentHeight());
}

void ListNode::scrollToBottom() {
    auto cl = m_scrollLayer->m_contentLayer;
    cl->setPositionY(0);
}

void ListNode::setOverscrollEnabled(bool enabled) {
    m_overscroll = enabled;
}

void ListNode::updateLayout() {
    m_scrollLayer->m_contentLayer->updateLayout();

    // update cell colors
    for (size_t i = 0; i < this->size(); i++) {
        auto cell = this->getCell(i);
        auto col = this->getCellColor(i);
        cell->setColor(into<ccColor3B>(col));
        cell->setOpacity(col.a);
        cell->setZOrder(i);
    }
}

ScrollPos ListNode::getScrollPos() {
    auto cl = m_scrollLayer->m_contentLayer;
    if (cl->getPositionY() > 0.f) return ScrollPos();
    return ScrollPos(cl->getScaledContentHeight() + cl->getPositionY());
}

void ListNode::setScrollPos(ScrollPos pos) {
    if (pos.atBottom) return;

    auto cl = m_scrollLayer->m_contentLayer;
    float actualPos = pos.val - cl->getScaledContentHeight();

    cl->setPositionY(std::min(actualPos, 0.f));
}

bool ListNode::isAtTop() {
    return std::abs(m_scrollLayer->m_contentLayer->getPositionY()) < 0.01f;
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