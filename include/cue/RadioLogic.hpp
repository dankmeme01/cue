#pragma once
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/function.hpp>
#include <variant>

namespace cue {

template <typename Val = std::monostate>
class RadioLogic {
private:
    static constexpr const char* OBJECT_ID = "dankmeme.cue/RadioLogic";
    static constexpr bool EnumMode = !std::is_same_v<Val, std::monostate>;
    using SelIndex = std::conditional_t<EnumMode, Val, size_t>;

    struct Toggler {
        geode::WeakRef<CCMenuItemToggler> m_toggler;
        SelIndex m_index{};
        geode::Function<void(CCMenuItemToggler*)> m_callback;

        Toggler(decltype(m_toggler) toggler, SelIndex index, decltype(m_callback) callback)
            : m_toggler(std::move(toggler)), m_index(index), m_callback(std::move(callback)) {}
    };

    // We store this in actual togglers as user objects, so it unfortunately cannot be a shared ptr
    struct Impl : cocos2d::CCObject {
        SelIndex m_selected{};
        std::vector<Toggler> m_togglers;
        geode::Function<void(CCMenuItemToggler*, SelIndex)> m_callback;

        void onClicked(CCMenuItemToggler* toggler) {
            this->select(toggler, true);
        }

        void select(CCMenuItemToggler* toggler, bool callback) {
            auto it = std::find_if(m_togglers.begin(), m_togglers.end(), [toggler](const auto& t) {
                return t.m_toggler.lock() == toggler;
            });

            if (it == m_togglers.end()) {
                return;
            }

            this->select(*it, callback);
        }

        void select(SelIndex id, bool callback) {
            auto it = std::find_if(m_togglers.begin(), m_togglers.end(), [id](const auto& t) {
                return t.m_index == id;
            });

            if (it == m_togglers.end()) {
                return;
            }

            this->select(*it, callback);
        }

        void select(Toggler& t, bool callback) {
            bool changed = m_selected != t.m_index;

            m_selected = t.m_index;
            if (t.m_callback) t.m_callback(t.m_toggler.lock());

            // toggle the other togglers
            for (auto& other : m_togglers) {
                auto toggler = other.m_toggler.lock();
                if (!toggler) continue;

                if (callback) {
                    toggler->toggle(false); // magic
                } else {
                    toggler->toggle(other.m_index == t.m_index);
                }
            }

            // call the main callback
            if (m_callback && changed) {
                m_callback(t.m_toggler.lock(), t.m_index);
            }
        }
    };

    geode::Ref<Impl> m_impl;

public:
    RadioLogic() : m_impl(geode::Ref<Impl>::adopt(new Impl{})) {}
    RadioLogic(const RadioLogic& other) = default;
    RadioLogic& operator=(const RadioLogic& other) = default;
    RadioLogic(RadioLogic&& other) noexcept = default;
    RadioLogic& operator=(RadioLogic&& other) noexcept = default;

    SelIndex getSelected() const noexcept {
        return m_impl->m_selected;
    }

    void select(SelIndex index) noexcept {
        m_impl->select(index, false);
    }

    void setCallback(geode::Function<void(CCMenuItemToggler*, SelIndex)> callback) {
        m_impl->m_callback = std::move(callback);
    }

    void setCallback(geode::Function<void(CCMenuItemToggler*)> callback) {
        m_impl->m_callback = [callback = std::move(callback)](CCMenuItemToggler* toggler, SelIndex) mutable {
            callback(toggler);
        };
    }

    void setCallback(geode::Function<void(SelIndex)> callback) {
        m_impl->m_callback = [callback = std::move(callback)](CCMenuItemToggler*, SelIndex index) mutable {
            callback(index);
        };
    }

    // Enum mode

    auto createToggler(SelIndex tag, float scale = 1.0f, geode::Function<void(CCMenuItemToggler*)> callback = {}) requires (EnumMode) {
        auto toggler = geode::cocos::CCMenuItemExt::createTogglerWithStandardSprites(scale, +[](CCMenuItemToggler* toggler) {
            auto impl = static_cast<Impl*>(toggler->getUserObject(OBJECT_ID));
            impl->onClicked(toggler);
        });
        toggler->setUserObject(OBJECT_ID, m_impl);

        m_impl->m_togglers.emplace_back(toggler, tag, std::move(callback));
        if (m_impl->m_togglers.size() == 1) {
            // select this toggler
            m_impl->select(m_impl->m_togglers.front(), false);
        }

        return toggler;
    }

    // Plain mode

    auto createToggler(float scale = 1.0f, geode::Function<void(CCMenuItemToggler*)> callback = {}) requires (!EnumMode) {
        auto toggler = geode::cocos::CCMenuItemExt::createTogglerWithStandardSprites(scale, +[](CCMenuItemToggler* toggler) {
            auto impl = static_cast<Impl*>(toggler->getUserObject(OBJECT_ID));
            impl->onClicked(toggler);
        });
        toggler->setUserObject(OBJECT_ID, m_impl);

        m_impl->m_togglers.emplace_back(toggler, m_impl->m_togglers.size(), std::move(callback));
        if (m_impl->m_togglers.size() == 1) {
            // select this toggler
            m_impl->select(m_impl->m_togglers.front(), false);
        }

        return toggler;
    }
};

}
