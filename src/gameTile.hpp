#pragma once

#include "selectionSet.hpp"
#include "button.hpp"

class GameTile: public Button{
private:
    bool isMarked;
    SelectionSet* rowSet;
public:
    GameTile(Vector2 position, float tileWidth, float tileHeight, SelectionSet* rowSet);
    ~GameTile();

    void drawMarked();

    void mark();
    void unmark();
    bool getIsMarked();
};