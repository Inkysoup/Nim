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

    Timer sumbitTimer;
    float submitDelaySeconds;
    Timer computerTimer;
    float computerDelaySeconds;

    Shader matchShader;
    Shader selectedFireShader;
    Vector4 fireColour;

    int getRowByMSB(unsigned bitmask);
    int getNonSingleRow();
    //void disableTiles(int rowIndex, unsigned count);
    void disableSelected();
public:
    GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileWidth, float tileHeight);
    ~GameBoard();
    void Draw();

    void update();

    void submit();
    void setSelectedRow(SelectionSet* row);
    void selectTiles(int rowIndex, unsigned count);
    void computerMove();
    void resetState();

    bool getIsPaused();
};