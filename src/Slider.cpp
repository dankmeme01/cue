#include <cue/Slider.hpp>

using namespace geode::prelude;

constexpr float FILL_PAD = 2.f;
constexpr float KNOB_PAD = 4.f;

namespace cue {

using Thumb = Slider::Thumb;

static double snapToStep(double value, double step) {
    if (step <= 0.0) return value;

    double stepCount = std::round(value / step);
    return stepCount * step;
}

bool Slider::init(
    const char* start,
    const char* middle,
    const char* end,
    const char* fill,
    const char* thumb,
    const char* thumbSelected
) {
    if (!ProgressBar::init(start, middle, end, fill)) return false;

    m_thumb = Thumb::create(this, thumb, thumbSelected);
    m_thumb->setZOrder(10);
    m_thumb->setScale(0.6f);
    m_thumb->setID("thumb");
    m_thumb->setAnchorPoint({0.5f, 0.5f});
    this->addChild(m_thumb);

    // initialize
    this->setup(this->getContentSize());

    return true;
}

void Slider::setContentSize(const CCSize& size) {
    if (!m_thumb) {
        ProgressBar::setContentSize(size);
        return;
    }

    this->setup(size);
}

void Slider::setup(CCSize size) {
    ProgressBar::setup(size);

    m_thumb->setPositionY(m_outlineStart->getScaledContentHeight() / 2.f);

    // force update x pos
    this->setValueRaw(this->getValueRaw());
}

void Slider::setRange(double min, double max) {
    if (min > max) return;

    m_rangeStart = min;
    m_rangeEnd = max;
}

void Slider::setCallback(Callback&& cb) {
    callback = std::move(cb);
}

void Slider::setCallback(const Callback& cb) {
    callback = cb;
}

double Slider::getValue() {
    double rawv = this->getValueRaw();
    double scaled = m_rangeStart + (m_rangeEnd - m_rangeStart) * rawv;

    return snapToStep(scaled, this->scaledStep());
}

double Slider::getValueRaw() {
    return ProgressBar::getValue();
}

void Slider::setValue(double value) {
    double diff = m_rangeEnd - m_rangeStart;
    double step = this->scaledStep();
    double unsnapped = std::clamp((value - m_rangeStart) / diff, 0.0, 1.0);

    // snap to step and set
    this->setValueRaw(snapToStep(unsnapped, step));
}

void Slider::setValueRaw(double value) {
    ProgressBar::setValue(value);

    m_thumb->setPositionX(KNOB_PAD + (this->getContentWidth() - KNOB_PAD * 2) * ProgressBar::getValue());
}

void Slider::setStep(double step) {
    m_step = step;
}

bool Slider::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    CCMenu::ccTouchBegan(touch, event);

    if (!cocos::nodeIsVisible(this)) {
        return false;
    }

    // ok so
    auto relpos = this->convertTouchToNodeSpace(touch);
    auto knobSize = m_thumb->getScaledContentSize();
    auto knobpos = m_thumb->getPosition() - knobSize / 2.f;

    // check if it's inside of the rect
    if (
        relpos.x > knobpos.x
        && relpos.y > knobpos.y
        && relpos.x < (knobpos.x + knobSize.width)
        && relpos.y < (knobpos.y + knobSize.height)
    ) {
        m_thumb->setState(true);
        float middle = knobpos.x + knobSize.width / 2.f;

        m_thumbCorrection = -(relpos.x - middle);
        return true;
    }

    return false;
}

void Slider::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    CCMenu::ccTouchEnded(touch, event);
    m_thumb->setState(false);
}

void Slider::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    CCMenu::ccTouchMoved(touch, event);

    if (!m_thumb->isHeld()) {
        return;
    }

    // move the slider knob if it's selected

    auto relX = this->convertTouchToNodeSpace(touch).x - KNOB_PAD;
    float maxw = this->getContentSize().width - KNOB_PAD * 2;

    relX = std::clamp(relX + m_thumbCorrection, 0.f, maxw);
    double rawFill = relX / maxw;

    this->setValueRaw(snapToStep(rawFill, this->scaledStep()));

    if (callback) {
        callback(this, this->getValue());
    }
}

Slider* Slider::create(
    const char* start,
    const char* middle,
    const char* end,
    const char* fill,
    const char* thumb,
    const char* thumbSelected
) {
    auto ret = new Slider;
    if (ret->init(start, middle, end, fill, thumb, thumbSelected)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool Thumb::init(Slider* slider, const char* tex, const char* texsel) {
    m_slider = slider;
    m_offSprite = CCSprite::create(tex);
    this->addChild(m_offSprite);

    m_onSprite = CCSprite::create(texsel);
    m_onSprite->setVisible(false);
    this->addChild(m_onSprite);

    auto sz = m_onSprite->getScaledContentSize();
    this->setScaledContentSize(sz);

    m_offSprite->setPosition(sz / 2.f);
    m_onSprite->setPosition(sz / 2.f);

    return true;
}

void Thumb::setState(bool held) {
    m_offSprite->setVisible(!held);
    m_onSprite->setVisible(held);
}

bool Thumb::isHeld() {
    return m_onSprite->isVisible();
}

Thumb* Thumb::create(Slider* slider, const char* tex, const char* texsel) {
    auto ret = new Thumb;
    if (ret->init(slider, tex, texsel)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

}
