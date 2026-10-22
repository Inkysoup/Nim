#include "gameBoard.hpp"

#include <iostream>

GameBoard::GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileDiameter):
        ContainerBox(position, boardWidth, boardHeight){
    int rowCount = 5;
    //float absTileRadius = ContainerBox::getScreenMagnitude()*tileRadius;

    float yTileGap = (boardHeight - (tileDiameter*rowCount))/(rowCount - 1.0f); 
    float relativeYTileGap = yTileGap / boardHeight;
    float relativeTileDiameter = tileDiameter / boardHeight;

    int maxTileCount = (rowCount-1)*2 + 1;
    float xTileGap = (boardWidth - (tileDiameter*maxTileCount))/(maxTileCount - 1.0f); 
    float relativeXTileGap = xTileGap / boardWidth;

    for(int i = 0; i<rowCount; ++i){
        float yPos = i*(relativeTileDiameter + relativeYTileGap);
        
        int tileCount = 1 + 2*i;
        std::vector<GameTile*> rowTiles;
        for(int j = 0; j<tileCount; ++j){
            float xPos = j*(relativeTileDiameter + relativeXTileGap);
            auto gt = std::make_unique<GameTile>(Vector2{xPos, yPos}, tileDiameter);
            rowTiles.push_back(static_cast<GameTile*>(this->addChild(std::move(gt))));
        }
        gameTiles.push_back(rowTiles);
    }
}

GameBoard::~GameBoard(){
}

void GameBoard::Draw(){
    drawDebug();
    // for(auto rowTiles: gameTiles){
    //     for(GameTile* tile: rowTiles){
    //         tile->drawDebug(GREEN);
    //     }
    // }
}

void GameBoard::computerMove(){
    // int largestRowIndex = 0;
    // int largestCount = 0;
    // for(unsigned i = 0; i < gameTiles.size(); ++i){
    //     int count = 0;
    //     for(GameTile* tile: gameTiles[i]){
    //         if(!tile->getIsDisabled()) ++count;
    //     }
    //     if(count > largestCount){
    //         largestRowIndex = i;
    //         largestCount = count;
    //     }
    // }

    std::vector<int> rowSums;
    int nimSum = 0;
    for(auto row: gameTiles){
        int rowSum = 0;
        for(GameTile* tile: row){
            if(!tile->getIsDisabled()) rowSum += 1; 
        }
        rowSums.push_back(rowSum);
        nimSum ^= rowSum; // bitwise xor
    }
    
    std::cout << "pnimsum: " << nimSum;

    if(nimSum != 0){ // computer wins
        for(unsigned rowIndex = 0; rowIndex<gameTiles.size(); ++rowIndex){
            std::cout << " row" << rowIndex<< ": " << rowSums[rowIndex];
            if(rowSums[rowIndex] >= nimSum){
                int i = 0;
                while(nimSum > 0){
                    GameTile* tile = gameTiles[rowIndex][i];
                    if(!tile->getIsDisabled()){
                        gameTiles[rowIndex][i]->disable();
                        --nimSum;
                    }
                    ++i;
                }
            }
        }
    }
    else{ // computer loses

    }
    std::cout << " nnimsum: " << nimSum <<"\n";

}

void GameBoard::resetState(){
    for(auto rowTiles: gameTiles){
        for(GameTile* tile: rowTiles){
            tile->enable();
        }
    }   
}
