#include <cue/PlayerIcon.hpp>

using namespace geode::prelude;

namespace cue {

bool PlayerIcon::init(const Icons& icons) {
    if (!SimplePlayer::init(0)) return false;

    this->setContentSize(m_outlineSprite->getScaledContentSize());
    this->updateIcons(icons);

    return true;
}

void PlayerIcon::updateIcons(const Icons& icons) {
    m_icons = icons;
    this->updateIcons();
}

void PlayerIcon::updateIcons() {
    static auto gm = GameManager::get();

    this->updatePlayerFrame(m_icons.id, m_icons.type);
    this->setColor(gm->colorForIdx(m_icons.color1));
    this->setSecondColor(gm->colorForIdx(m_icons.color2));

    if (m_icons.glowColor == -1) {
        this->disableGlowOutline();
    } else {
        this->setGlowOutline(gm->colorForIdx(m_icons.glowColor));
    }

    for (auto child : CCArrayExt<CCNode>(this->getChildren())) {
        child->setPosition(this->getContentSize() / 2.f);
    }
}

PlayerIcon* PlayerIcon::create(const Icons& icons) {
    auto ret = new PlayerIcon();
    if (ret->init(icons)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

}
