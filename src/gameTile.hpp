#pragma once

#include "gameTileSelectionSet.hpp"
#include "button.hpp"

class GameTile: public Button{
private:
    bool isMarked;
    GameTileSelectionSet* rowSet;
public:
    GameTile(Vector2 position, float tileWidth, float tileHeight, GameTileSelectionSet* rowSet);
    ~GameTile();

    void drawMarked();

    void mark();
    void unmark();
    bool getIsMarked();
};