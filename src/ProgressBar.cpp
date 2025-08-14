#include <cue/ProgressBar.hpp>

using namespace geode::prelude;

namespace cue {

constexpr static float FILL_PAD = 2.f;

// this is kinda bad but eh whatever
constexpr static float OUTLINE_SCALE = 0.69f;

bool ProgressBar::init(
        const char* start,
        const char* middle,
        const char* end,
        const char* fill
) {
    if (!CCMenu::init()) return false;

    // stupid ass cocos conventions why do i have to do that
    this->ignoreAnchorPointForPosition(false);

    m_outlineStart = CCSprite::create(start);
    m_outlineStart->setScale(OUTLINE_SCALE);
    m_outlineStart->setZOrder(5);
    m_outlineStart->setAnchorPoint({0.f, 0.5f});
    m_outlineStart->setID("outline-start");
    this->addChild(m_outlineStart);

    m_outlineEnd = CCSprite::create(end);
    m_outlineEnd->setScale(OUTLINE_SCALE);
    m_outlineEnd->setZOrder(5);
    m_outlineEnd->setAnchorPoint({1.f, 0.5f});
    m_outlineEnd->setID("outline-end");
    this->addChild(m_outlineEnd);

    ccTexParams tp = {
        GL_LINEAR, GL_LINEAR,
        GL_REPEAT, GL_CLAMP_TO_EDGE
    };

    m_outlineMiddle = CCSprite::create(middle);
    m_outlineMiddle->setScale(OUTLINE_SCALE);
    m_outlineMiddle->setZOrder(4);
    m_outlineMiddle->setAnchorPoint({0.5f, 0.5f});
    m_outlineMiddle->setID("outline-middle");
    m_outlineMiddle->getTexture()->setTexParameters(&tp);
    this->addChild(m_outlineMiddle);

    m_fill = CCSprite::create(fill);
    m_fill->setScaleY(0.7f);
    m_fill->setZOrder(1);
    m_fill->setAnchorPoint({0.f, 0.5f});
    m_fill->setID("fill");
    m_fill->getTexture()->setTexParameters(&tp);
    this->addChild(m_fill);

    // initialize
    this->setContentSize({64.f, 0.f});
    this->setValue(0.0);

    return true;
}

void ProgressBar::setContentSize(const CCSize& size) {
    if (!m_outlineStart) {
        CCMenu::setContentSize(size);
        return;
    }

    this->setup(size);
}

void ProgressBar::setup(CCSize size) {
    float width = size.width;
    float height = m_outlineStart->getScaledContentHeight();

    float outlineMiddleWidth = std::max(0.f, width - m_outlineStart->getScaledContentWidth() - m_outlineEnd->getScaledContentWidth());

    m_outlineMiddle->setTextureRect({0.f, 0.f, outlineMiddleWidth / OUTLINE_SCALE, height / OUTLINE_SCALE});

    m_outlineStart->setPosition({0.f, height / 2.f});
    m_outlineEnd->setPosition({width, height / 2.f});
    m_outlineMiddle->setPosition({width / 2.f, height / 2.f});
    m_fill->setPosition({FILL_PAD, height / 2.f});

    CCMenu::setContentSize({width, height});
}

double ProgressBar::getValue() {
    return m_value;
}

void ProgressBar::setValue(double value) {
    m_value = std::clamp(value, 0.0, 1.0);

    // update fill
    float maxWidth = this->getContentWidth() - FILL_PAD;
    float range = maxWidth - FILL_PAD;

    float width = range * m_value;
    m_fill->setTextureRect({0.f, 0.f, width, m_outlineStart->getScaledContentHeight()});
}

ProgressBar* ProgressBar::create(
    const char* start,
    const char* middle,
    const char* end,
    const char* fill
) {
    auto ret = new ProgressBar;
    if (ret->init(start, middle, end, fill)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}


}
