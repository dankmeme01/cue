#pragma once

#include <Geode/Geode.hpp>
#include "Util.hpp"
#include "ListBorder.hpp"

namespace cue {

class ListNode;

enum class Justify {
    Left,
    Center,
    Right,
};

class ListCell : public cocos2d::CCLayerColor {
public:
    static ListCell* create(cocos2d::CCNode* cell, ListNode* list);

    inline CCNode* getInner() {
        return m_inner;
    }

private:
    CCNode* m_inner;

    bool init(CCNode* cell, ListNode* list);
    void draw() override;
};

class ListNode : public cocos2d::CCLayer {
public:
    static ListNode* create(cocos2d::CCSize size, cocos2d::ccColor4B bgColor, ListBorderStyle borderStyle = ListBorderStyle::Default);

    size_t size();
    ListCell* getCell(size_t index);
    cocos2d::CCArray* getCells();

    // Add an arbitrary type as a cell to the list
    // Equivalent to `addListCell(ListCell::create(cell))`
    void addCell(CCNode* cell);

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

    void updateLayout();

    // TODO: get/set scroll position

    template <typename T>
    void setCellColor(const T& in) {
        m_evenColor = m_oddColor = cue::into<cocos2d::ccColor4B>(in);
    }

    template <typename T, typename Y>
    void setCellColors(const T& even, const Y& odd) {
        m_evenColor = cue::into<cocos2d::ccColor4B>(even);
        m_oddColor = cue::into<cocos2d::ccColor4B>(odd);
    }

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
    cocos2d::CCSize m_size;
    float m_cellHeight = 0.0f;
    Justify m_justify = Justify::Left;
    bool m_autoUpdate = true;

    bool init(cocos2d::CCSize size, cocos2d::ccColor4B bgColor, ListBorderStyle borderStyle);

    cocos2d::ccColor4B getCellColor(size_t index);

public:
    // iterator impl
    template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using held_type = ListCell*;
        using value_type = T*;
        using reference = T*&;
        using pointer = T**;

        iterator(cocos2d::CCArray* children, size_t index) : m_array(children), m_index(index) {}
        value_type operator*() const { return get(); }
        value_type operator->() const { return get(); }
        iterator& operator++() {
            ++m_index;
            return *this;
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

    template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
    struct Iter {
        ListNode* m_list;
        using iterator = ListNode::iterator<T>;

        iterator begin() {
            return iterator(m_list->getCells(), 0);
        }

        iterator end() {
            return iterator(m_list->getCells(), m_list->size());
        }
    };

    template <typename T> requires (std::derived_from<T, cocos2d::CCNode>)
    Iter<T> iter() {
        return Iter<T>{this};
    }
};

}
