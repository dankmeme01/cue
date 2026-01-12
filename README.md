# cue

cue (short for cocos UI extensions) is a library for Geode mods that adds helpful and convenient UI nodes. WIP.

Currently available:

* `cue::ListNode` - an extensive list class with user friendly interface. See below for a more in-depth showcase.
* `cue::DropdownNode` - an expandable list that can have dynamic and arbitrary contents.
* `cue::RepeatingBackground` - a customizable repeating background, similar to the one used in GD levels and MenuLayer, or some Globed layers.
* `cue::attachBackground(node)` - a handy util function that attaches a slightly dark `CCScale9Sprite` behind the given node. All the details about it are customizable.
* plus some other useful helper functions!
* `cue::PlayerIcon` - a simple wrapper around `SimplePlayer` that has a correct content size and makes it simple to set the displayed icon.
* `cue::LoadingCircle` - a properly functioning `LoadingCircle`
* `cue::ProgressBar` - a simple resizable progress bar class, similar to the one GD has
* `cue::Slider` - a convenient slider class, allows to set value range and resize freely, whereas the vanilla `Slider` class has issues with that

## Adding cue

Add the following lines to your CMakeLists.txt

```cmake
CPMAddPackage("gh:dankmeme01/cue#main") # prefer a specific commit over main
target_link_libraries(${PROJECT_NAME} PRIVATE cue)
```

# In-depth overviews

## ListNode

`cue::ListNode` is one of the easiest ways to create your own list element. For example here's how you would create a simple list of labels, in the comments style:

```cpp
CCSize listSize { 240.f, 200.f };

auto list = cue::ListNode::create(listSize, cue::Brown, cue::ListBorderStyle::Comments);

for (size_t i = 0; i < 10; i++) {
    list->addCell(CCLabelBMFont::create(fmt::format("Test {}", i).c_str(), "bigFont.fnt"));
}
```

Performing various operations on the list is also very simple. Here are some examples:

Adding, removing, retrieving cells
```cpp
// insert a new cell at the end
auto cell = list->addCell(CCNode::create());

// insert a new cell at the start
list->insertCell(CCNode::create(), 0);

// remove a cell
list->removeCell(cell);

// or do it by index!
list->removeCell(list->indexForCell(cell));
list->removeCell(0);

// remove all cells
list->clear();
```

Sorting and shuffling
```cpp
// shuffles the list, NOT IMPLEMENTED YET
list->shuffle();

// sorts the list with your custom comparator
list->sortAs<CCLabelBMFont>([&](CCLabelBMFont* a, CCLabelBMFont* b) {
    return a->getZOrder() < b->getZOrder();
});
```

Set how to align the list contents if they don't take the entire list width
```cpp
list->setJustify(cue::Justify::Left);  // elements will be aligned to the left side, default
list->setJustify(cue::Justify::Right);  // elements will be aligned to the right side
list->setJustify(cue::Justify::Center);  // elements will be aligned to the center
```

`ListNode` automatically calls `this->updateLayout` whenever cells are added or removed. For massive lists, this can be a huge performance penalty. For that purpose, auto updating can be disabled.
```cpp
// disable auto update to prevent excessive layout calculations
list->setAutoUpdate(false);

// add a huge amount of nodes
for (size_t i = 0; i < 65536; i++) {
    list->addCell(CCNode::create());
}

// enable auto update back and update the list manually
list->setAutoUpdate(true);
list->updateLayout();
```
