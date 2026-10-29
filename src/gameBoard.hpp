#pragma once

#include <vector>

#include "containerBox.hpp"
#include "gameTile.hpp"
#include "selectionSet.hpp"


class GameBoard: public ContainerBox{
private:
    std::vector<std::unique_ptr<SelectionSet>> gameTiles;
    std::vector<unsigned> rowSums;
    SelectionSet* selectedRow;

    Shader matchShader;
    Shader selectedFireShader;

    int getRowByMSB(unsigned bitmask);
    int getNonSingleRow();
    void disableTiles(int rowIndex, unsigned count);
public:
    GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileWidth, float tileHeight);
    ~GameBoard();
    void Draw();

    void submit();
    void setSelectedRow(SelectionSet* row);
    void computerMove();
    void resetState();
};