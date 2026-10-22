#pragma once

#include "button.hpp"

class GameTile: public Button{
private:
public:
    GameTile(Vector2 position, float tileDiameter);
    ~GameTile();
};