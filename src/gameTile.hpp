#pragma once

#include "selectionSet.hpp"
#include "button.hpp"

class GameTile: public Button{
private:
public:
    GameTile(Vector2 position, float tileWidth, float tileHeight, SelectionSet* rowSet);
    ~GameTile();
};