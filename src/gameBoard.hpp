#pragma once

#include <vector>

#include "containerBox.hpp"
#include "gameTile.hpp"


class GameBoard: public ContainerBox{
private:
    std::vector<std::vector<GameTile*>> gameTiles;
public:
    GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileDiameter);
    ~GameBoard();
    void Draw();

    void computerMove();
    void resetState();
};