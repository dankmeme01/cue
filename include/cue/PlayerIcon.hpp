#pragma once

#include <Geode/Geode.hpp>

namespace cue {

struct Icons {
    IconType type = IconType::Cube;
    int id = 1;
    int color1 = 1, color2 = 3;
    int glowColor = -1; // -1 = no glow, >= 0 = glow with color
};

class PlayerIcon : public SimplePlayer {
public:
    static PlayerIcon* create(const Icons& icons);

    inline static PlayerIcon* create(IconType type, int id = 1, int color1 = 1, int color2 = 3, int glowColor = -1) {
        return create(Icons{type, id, color1, color2, glowColor});
    }

    void updateIcons(const Icons& icons);

protected:
    Icons m_icons;

    bool init(const Icons& icons);
    void updateIcons();
};

}
