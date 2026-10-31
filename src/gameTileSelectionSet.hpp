#pragma once

#include "selectionSet.hpp"

class GameTileSelectionSet: public SelectionSet{
private:
public:
    GameTileSelectionSet();
    ~GameTileSelectionSet();

    void handleKeyboardInput(int keycode) override;
};