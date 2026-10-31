#pragma once

#include <vector>
#include <functional>

#include "containerBox.hpp"
#include "gameTile.hpp"
#include "gameTileSelectionSet.hpp"


class GameBoard: public ContainerBox{
private:
    std::vector<std::unique_ptr<GameTileSelectionSet>> gameTiles;
    std::vector<unsigned> rowSums;
    GameTileSelectionSet* selectedRow;

    Timer sumbitTimer;
    float burnDelaySeconds;
    Timer computerTimer;
    float computerDelaySeconds;

    Shader matchShader;
    Vector4 fireColour;

    std::function<void(bool win)> onEndfunc;

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
    void setSelectedRow(GameTileSelectionSet* row);
    void selectTiles(int rowIndex, unsigned count);
    void computerMove();
    void resetState();

    void bindOnEnd(std::function<void(bool win)> func); // true = win, false = loss
    bool isGameOver();

    bool getIsPaused();
};