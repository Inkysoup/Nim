#pragma once

#include "selectionSet.hpp"
#include "button.hpp"

class GameTile: public Button{
private:
    bool isMarked;
public:
    GameTile(Vector2 position, float tileWidth, float tileHeight, SelectionSet* rowSet);
    ~GameTile();

    void Draw();

    void mark();
    void unmark();
    bool getIsMarked();
};