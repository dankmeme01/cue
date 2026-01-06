#pragma once

#include <Geode/Geode.hpp>
#include "Util.hpp"
#include "ListBorder.hpp"
#include "Colors.hpp"

namespace cue {

class ListNode;

enum class Justify {
    Left,
    Center,
    Right,
};

struct ScrollPos {
public:
    bool operator==(const ScrollPos&) const = default;


private:
    friend class ListNode;
    friend class DropdownNode;

    float val = 0.0f;
    bool atBottom = false;

    ScrollPos(float val) : val(val), atBottom(false) {}
    ScrollPos() : val(0.0f), atBottom(true) {}

    operator float() const { return val; }
};

class ListCell : public cocos2d::CCLayerColor {
public:
    static ListCell* create(CCNode* cell, ListNode* list);

    inline CCNode* getInner() {
        return m_inner;
    }

private:
    CCNode* m_inner;
    CCNode* m_bg;

    bool init(CCNode* cell, ListNode* list);
    void draw() override;
};

class ListNode : public cocos2d::CCLayer {
public:
    /// Create a `ListNode` with specified size, background color, and border style.
    /// By default is equivalent to `createComments`.
    static ListNode* create(cocos2d::CCSize size, cocos2d::ccColor4B bgColor = Brown, ListBorderStyle borderStyle = ListBorderStyle::Comments);

    /// Create a `ListNode` styled for comments, using a brown background and comment border style.
    static ListNode* createComments(cocos2d::CCSize size);

    /// Create a `ListNode` styled for levels, using a brown background and the green-blue border style seen in the game.
    static ListNode* createLevels(cocos2d::CCSize size);

    size_t size();
    ListCell* getCell(size_t index);
    cocos2d::CCArray* getCells();

    // Add an arbitrary type as a cell to the list
    // Equivalent to `addListCell(ListCell::create(cell))`
    ListCell* addCell(CCNode* cell);

    // Add a ListCell to the list
    void addListCell(ListCell* cell);

    // Insert an arbitrary type as a cell to the list at the specified index
    // Equivalent to `insertListCell(ListCell::create(cell), index)`
    void insertCell(CCNode* cell, size_t index);

    // Insert a ListCell to the list at the specified index
    void insertListCell(ListCell* cell, size_t index);

    // Remove a cell at the specified index
    void removeCell(size_t index);
    void removeCell(ListCell* cell);

    // Retrieve the index of a cell
    size_t indexForCell(ListCell* cell);

    // Remove all cells from the list
    void clear();

    // Sort the list using a custom comparator
    template <typename F>
        requires (std::convertible_to<std::invoke_result_t<F, ListCell*, ListCell*>, bool>)
    void sort(F&& cmp) {
        // sorting a ccarray is very annoying, so we cheat a little
        std::vector<geode::Ref<ListCell>> cells;

        for (auto child : geode::cocos::CCArrayExt<ListCell>(this->getCells())) {
            cells.push_back(child);
        }

        std::sort(cells.begin(), cells.end(), std::forward<F>(cmp));

        for (size_t i = 0; i < cells.size(); i++) {
            auto& cell = cells[i];
            cell->setZOrder(i);
        }

        if (m_autoUpdate) {
            this->updateLayout();
        }
    }

    // Sort the list using a custom comparator and custom cell class
    template <typename T, typename F>
        requires (std::convertible_to<std::invoke_result_t<F, T*, T*>, bool>
                && std::derived_from<T, cocos2d::CCNode>)
    void sortAs(F&& cmp) {
        this->sort([&](ListCell* a, ListCell* b) {
            return cmp(static_cast<T*>(a->getInner()), static_cast<T*>(b->getInner()));
        });
    }

    void shuffle();

    void scrollToTop();
    void scrollToBottom();

    void setOverscrollEnabled(bool enabled);

    void updateLayout(bool preserveScroll = true);

    ScrollPos getScrollPos();
    void setScrollPos(ScrollPos pos);

    bool isAtTop();

    template <typename T>
    void setCellColor(const T& in) {
        m_evenColor = m_oddColor = cue::into<cocos2d::ccColor4B>(in);
    }

    template <typename T, typename Y>
    void setCellColors(const T& even, const Y& odd) {
        m_evenColor = cue::into<cocos2d::ccColor4B>(even);
        m_oddColor = cue::into<cocos2d::ccColor4B>(odd);
    }

    void setFullHeight(float height);

    inline void setCellHeight(float height) {
        m_cellHeight = height;
    }

    inline cocos2d::CCSize getListSize() {
        return m_size;
    }

    inline void setAutoUpdate(bool autoUpdate) {
        m_autoUpdate = autoUpdate;
    }

    inline void setJustify(Justify justify) {
        m_justify = justify;
    }

protected:
    friend class ListCell;

    geode::ScrollLayer* m_scrollLayer;
    geode::Ref<ListBorder> m_border;
    cocos2d::ccColor4B m_evenColor, m_oddColor;
    cocos2d::CCLayerColor* m_bg;
    cocos2d::CCSize m_size;
    float m_cellHeight = 0.0f;
    Justify m_justify = Justify::Left;
    bool m_autoUpdate = true;
    bool m_overscroll = true;

    bool init(cocos2d::CCSize size, cocos2d::ccColor4B bgColor, ListBorderStyle borderStyle);
    void update(float dt);
    float contentSize();

    cocos2d::ccColor4B getCellColor(size_t index);

public:
    // iterator impl
    template <typename T, bool Checked> requires (std::derived_from<T, cocos2d::CCNode>)
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using held_type = ListCell*;
        using value_type = T*;
        using reference = T*&;
        using pointer = T**;

        iterator(cocos2d::CCArray* children, size_t index) : m_array(children), m_index(index) {
            this->skipInvalid();
        }

        value_type operator*() const { return get(); }
        value_type operator->() const { return get(); }
        iterator& operator++() {
            ++m_index;
            this->skipInvalid();

            return *this;
        }

        void skipInvalid() {
            if constexpr (Checked) {
                auto cnt = m_array ? m_array->count() : 0;

                while (m_index < cnt && !geode::cast::typeinfo_cast<T*>(this->get())) {
                    m_index++;
                }
            }
        }

        bool operator==(const iterator& other) const {
            return m_index == other.m_index && m_array == other.m_array;
        }

    private:
        cocos2d::CCArray* m_array;
        size_t m_index;

        value_type get() const {
            return static_cast<T*>(static_cast<held_type>(m_array->objectAtIndex(m_index))->getInner());
        }
    };

    template <typename T, bool Checked> requires (std::derived_from<T, cocos2d::CCNode>)
    struct Iter {
        ListNode* m_list;
        using iterator = ListNode::iterator<T, Checked>;

        iterator begin() {
            return iterator(m_list->getCells(), 0);
        }

        iterator end() {
            return iterator(m_list->getCells(), m_list->size());
        }
    };

    /// Returns an iterator over children, very similar to `CCArrayExt`.
    /// Use this if you are certain all cells are of the same type, because it uses `static_cast`.
    /// If you are not sure, use `iterChecked`, which will skip cells of a different type than given.
    template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
    Iter<T, false> iter() {
        return Iter<T, false>{this};
    }
    /// Returns an iterator over children, skipping cells that don't match the requested type.
    template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
    Iter<T, true> iterChecked() {
        return Iter<T, true>{this};
    }
};

}
