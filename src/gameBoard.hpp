#pragma once

#include <vector>

#include "containerBox.hpp"
#include "gameTile.hpp"
#include "selectionSet.hpp"


class GameBoard: public ContainerBox{
private:
    std::vector<std::unique_ptr<SelectionSet>> gameTiles;
    std::vector<unsigned> rowSums;

    unsigned getRowByMSB(unsigned bitmask);
    unsigned getNonSingleRow();
    void disableTiles(unsigned rowIndex, unsigned count);
public:
    GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileWidth, float tileHeight);
    ~GameBoard();
    void Draw();

    void computerMove();
    void resetState();
};