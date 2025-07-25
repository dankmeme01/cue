#include <cue/RepeatingBackground.hpp>

using namespace geode::prelude;

namespace cue {

static CCTexture2D* loadTexture(const char* texture) {
    auto fullPath = CCFileUtils::get()->fullPathForFilename(texture, false);
    if (fullPath.empty()) {
        log::debug("[cue] failed to find texture '{}'", texture);
        return nullptr;
    }

    auto pathKey = fmt::format("dankmeme.cue-repeating-bg-tex-{}", texture);
    auto tc = CCTextureCache::get();

    auto tex = static_cast<CCTexture2D*>(tc->m_pTextures->objectForKey(pathKey));
    if (!tex) {
        auto img = new CCImage();
        img->initWithImageFile(fullPath.c_str());

        tex = new CCTexture2D();

        if (tex->initWithImage(img)) {
            tc->m_pTextures->setObject(tex, pathKey);
            tex->release();
        } else {
            tex->release();
            tex = nullptr;
        }

        img->release();
    }

    return tex;
}

bool RepeatingBackground::init(const char* texture, float scale, RepeatMode mode, cocos2d::CCSize visibleSize) {
    // Because we modify texture parameters, we don't just use CCSprite::init or anything like that,
    // we create our own texture to not taint the global one.
    auto tex = loadTexture(texture);

    if (!tex || !this->initWithTexture(tex)) return false;

    m_mode = mode;
    m_speed = 1.0f;
    m_visibleSize = visibleSize;

    auto winSize = CCDirector::get()->getWinSize();
    if (m_visibleSize.isZero()) {
        m_visibleSize = winSize;
    }

    auto bgrect = this->getTextureRect();
    auto rawTextureSize = this->getContentSize();

    bool repeatX = (mode == RepeatMode::X || mode == RepeatMode::Both);
    bool repeatY = (mode == RepeatMode::Y || mode == RepeatMode::Both);

    if (repeatX) {
        float minX = m_visibleSize.width * 3.f / scale;

        bgrect.size.width = 0;
        while (bgrect.size.width < minX) bgrect.size.width += rawTextureSize.width;
    }

    if (repeatY) {
        float minY = m_visibleSize.height * 2.f / scale;

        bgrect.size.height = 0;
        while (bgrect.size.height < minY) bgrect.size.height += rawTextureSize.height;
    }

    ccTexParams params = {
        GL_LINEAR, GL_LINEAR,
        (GLuint)(repeatX ? GL_REPEAT : GL_CLAMP_TO_EDGE),
        (GLuint)(repeatY ? GL_REPEAT : GL_CLAMP_TO_EDGE)
    };

    this->getTexture()->setTexParameters(&params);
    // this->setContentSize(winSize); // ?
    this->setTextureRect(bgrect);
    this->setAnchorPoint({0.5f, 0.5f});
    this->setScale(scale);
    this->scheduleUpdate();

    return true;
}

void RepeatingBackground::update(float dt) {
    bool repeatX = (m_mode == RepeatMode::X || m_mode == RepeatMode::Both);
    bool repeatY = (m_mode == RepeatMode::Y || m_mode == RepeatMode::Both);

    float moveByX = dt * m_speed * m_obContentSize.width / 12.f * (repeatX ? 1.f : 0.f);
    float moveByY = dt * m_speed * m_obContentSize.height / 15.f * (repeatY ? 1.f : 0.f);

    float loopAfterXMin = m_visibleSize.width;
    float loopAfterX = 0.f;
    while (loopAfterX < loopAfterXMin) {
        loopAfterX += m_obContentSize.width * this->getScale();
    }

    float loopAfterYMin = m_visibleSize.height;
    float loopAfterY = 0.f;
    while (loopAfterY < loopAfterYMin) {
        loopAfterY += m_obContentSize.height * this->getScale();
    }

    auto newPos = this->getPosition() - CCPoint{moveByX, moveByY};

    if (std::abs(newPos.x) > loopAfterX) {
        newPos.x += m_obContentSize.width * this->getScale();
    }

    if (std::abs(newPos.y) > loopAfterY) {
        newPos.y += m_obContentSize.height * this->getScale();
    }

    this->setPosition(newPos);
}

RepeatingBackground* RepeatingBackground::create(const char* texture, float scale, RepeatMode mode, cocos2d::CCSize visibleSize) {
    auto ret = new RepeatingBackground();
    if (ret->init(texture, scale, mode, visibleSize)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

}
