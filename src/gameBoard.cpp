#include "gameBoard.hpp"

#include <bit>
#include <random>

#include <exception>
#include <iostream>

unsigned GameBoard::getRowByMSB(unsigned bitmask){
    for(unsigned i = 0; i<rowSums.size(); ++i){
        if((rowSums[i] & bitmask) != 0) return i;
    }
    std::terminate();
}
unsigned GameBoard::getNonSingleRow(){
    for(unsigned i = 0; i<rowSums.size(); ++i){
        if(rowSums[i] > 1 ) return i;
    }
    std::terminate();
}

void GameBoard::disableTiles(unsigned rowIndex, unsigned count){
    int i = 0;
    auto rowSelectionSet = gameTiles[rowIndex]->getMembers();
    while(count > 0){
        if(auto* tile = dynamic_cast<GameTile*>(rowSelectionSet[i])) {
            if(!tile->getIsDisabled()){
                tile->disable();
                --count;
            }
            ++i;
        }
    }
    gameTiles[rowIndex]->resizeBoundingBox();
}

GameBoard::GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileWidth, float tileHeight):
        ContainerBox(position, boardWidth, boardHeight){
    int rowCount = 6;
    //float absTileRadius = ContainerBox::getScreenMagnitude()*tileRadius;

    float yTileGap = (boardHeight - (tileHeight*rowCount))/(rowCount - 1.0f); 
    float relativeYTileGap = yTileGap / boardHeight;
    float relativeTileHeight = tileHeight / boardHeight;

    int maxTileCount = (rowCount-1)*2 + 1;
    float xTileGap = (boardWidth - (tileWidth*maxTileCount))/(maxTileCount - 1.0f); 
    float relativeXTileGap = xTileGap / boardWidth;
    float relativeTileWidth = tileWidth / boardHeight;

    for(int i = 0; i<rowCount; ++i){
        float yPos = i*(relativeTileHeight + relativeYTileGap);
        
        int tileCount = 1 + 2*i;
        gameTiles.emplace_back(std::make_unique<SelectionSet>());
        SelectionSet* ss = &*gameTiles.back();
        for(int j = 0; j<tileCount; ++j){
            float xPos = j*(relativeTileWidth + relativeXTileGap);
            auto gt = std::make_unique<GameTile>(Vector2{xPos, yPos}, tileWidth, tileHeight, ss);
            gt->setAutoDraw(false);
            gt->setTabNavigatable(false);
            gameTiles.back()->addMember(addChild(std::move(gt)));
        }
    }
}

GameBoard::~GameBoard(){
}

void GameBoard::Draw(){
    for(const auto& rowSelectionSet: gameTiles){
        rowSelectionSet->drawDebug();
        for(ContainerBox* child: rowSelectionSet->getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                tile->Draw();
            }
        }
    }
}

void GameBoard::computerMove(){
    rowSums.clear();
    unsigned nimSum = 0;
    unsigned singlesRemaining = 0;
    unsigned nonSinglesRemaining = 0;
    for(const auto& row: gameTiles){
        unsigned rowSum = 0;
        for(ContainerBox* child: row->getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                if(!tile->getIsDisabled()) rowSum += 1;
            }
        }
        rowSums.push_back(rowSum);
        if(rowSum > 1) ++nonSinglesRemaining;
        else if(rowSum == 1) ++singlesRemaining;
        nimSum ^= rowSum; // bitwise xor
    }
    
    unsigned msbMask = std::bit_floor(nimSum);
    unsigned rowIndex;
    unsigned removeCount;
    if(nimSum != 0 or (nonSinglesRemaining == 0 and singlesRemaining%2 == 0)){ // computer wins
        if(nonSinglesRemaining > 1){
            rowIndex = getRowByMSB(msbMask);
            removeCount = rowSums[rowIndex] - (nimSum ^ rowSums[rowIndex]);
        }
        else if(nonSinglesRemaining == 1){ // entering endgame
            rowIndex = getNonSingleRow();
            // make the non-single row 1 or clear it in order to keep row count odd 
            if(singlesRemaining%2 == 0) removeCount = rowSums[rowIndex] - 1;
            else removeCount = rowSums[rowIndex];
        }
        else{ // forced moves in endgame
            rowIndex = getRowByMSB(1);
            removeCount = 1;
        }
    }
    else{ // computer loses
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rowDist(0, singlesRemaining + nonSinglesRemaining - 1);
        rowIndex = rowDist(gen);
        for(unsigned i = 0; i < rowSums.size(); ++i){
            if(rowSums[i] == 0) ++rowIndex;
            if(rowIndex == i) break;
        }
        std::uniform_int_distribution<> removeCountDist(1, rowSums[rowIndex]);
        removeCount = removeCountDist(gen);
    }
    disableTiles(rowIndex, removeCount);
}

void GameBoard::resetState(){
    for(const auto& rowSelectionSet: gameTiles){
        for(ContainerBox* child: rowSelectionSet->getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                tile->enable();
            }
        }
        rowSelectionSet->resizeBoundingBox();
    }
}
