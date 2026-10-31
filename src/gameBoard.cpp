#include "gameBoard.hpp"

#include "timer.hpp"

#include <bit>
#include <random>
#include <cassert>

#include <iostream>

#define MAX_TILES 64 // must be same as in matchShader.fs

int GameBoard::getRowByMSB(unsigned bitmask){
    for(unsigned i = 0; i<rowSums.size(); ++i){
        if((rowSums[i] & bitmask) != 0) return i;
    }
    return -1;
}
int GameBoard::getNonSingleRow(){
    for(unsigned i = 0; i<rowSums.size(); ++i){
        if(rowSums[i] > 1 ) return i;
    }
    return -1;
}

void GameBoard::disableSelected(){
    if(selectedRow == nullptr) return;

    int removedTiles = 0;
    for(ContainerBox* child: selectedRow->getMembers()){
        if(auto* tile = dynamic_cast<GameTile*>(child)) {
            if(tile->getIsMarked()){
                tile->unmark();
                tile->disable();
                ++removedTiles;
            }
        }
    }
    if(removedTiles == 0) return;
    selectedRow->resizeBoundingBox();
    selectedRow = nullptr;
}

GameBoard::GameBoard(Vector2 position, float boardWidth, float boardHeight, float tileWidth, float tileHeight):
        ContainerBox(position, boardWidth, boardHeight),
        sumbitTimer(0.0f),
        computerTimer(0.0f){
    burnDelaySeconds = 0.3f; // controls speed of burn animation
    computerDelaySeconds = 0.3f; // pause before computer move's burn animation is played 
    sumbitTimer.bindOnEnd([this](){ // onSumbit
        disableSelected();
        computerMove();
        fireColour = Vector4(0.0f, 0.5f, 0.95f, 1.0f); 
        computerTimer.setSecondsRemaining(computerDelaySeconds + burnDelaySeconds);
        computerTimer.resume();
    });
    
    computerTimer.bindOnEnd([this](){
        disableSelected();
        fireColour = Vector4(0.95f, 0.5f, 0.0f, 1.0f);
    });
    
    
    fireColour = Vector4(0.95f, 0.5f, 0.0f, 1.0f);

    selectedRow = nullptr;
    int rowCount = 5;

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
        gameTiles.emplace_back(std::make_unique<GameTileSelectionSet>());
        GameTileSelectionSet* ss = &*gameTiles.back();
        for(int j = 0; j<tileCount; ++j){
            float xPos = 0.5f + (j-tileCount/2.0f)*(relativeTileWidth) + (j-(tileCount-1)/2.0f) * relativeXTileGap;
            auto gt = std::make_unique<GameTile>(Vector2{xPos, yPos}, tileWidth, tileHeight, ss);
            gameTiles.back()->addMember(static_cast<GameTile*>(addChild(std::move(gt))));
        }
    }

    matchShader = LoadShader(0, TextFormat("src/matchShader.fs"));
}

GameBoard::~GameBoard(){
    UnloadShader(matchShader);
}

void GameBoard::Draw(){
    Rectangle tileRect = gameTiles[0]->getMembers()[0]->getRect();
    float headRadius = tileRect.width/4.0f;
    float outlineThickness = getOutlineWidth();
    
    auto setHeadCentreUniform = [](Shader shader, unsigned i, Vector2 headCentre){
        unsigned int headCentreUniformLocation = GetShaderLocation(shader, TextFormat("tiles[%d].headCentre", i));
        SetShaderValue(shader, headCentreUniformLocation, &headCentre, SHADER_UNIFORM_VEC2);
    };
    auto setIsMarkedUniform = [](Shader shader, unsigned i, bool isMarked){
        unsigned int isMarkedUniformLocation = GetShaderLocation(shader, TextFormat("tiles[%d].isMarked", i));
        int isMarkedInt = isMarked ? 1 : 0;
        SetShaderValue(shader, isMarkedUniformLocation, &isMarkedInt, SHADER_UNIFORM_INT);
    };


    BeginShaderMode(matchShader);
        // matches
        unsigned int headRadiusUniformLocation = GetShaderLocation(matchShader, "headRadius");
        SetShaderValue(matchShader, headRadiusUniformLocation, &headRadius, SHADER_UNIFORM_FLOAT);

        unsigned int stickRadiusUniformLocation = GetShaderLocation(matchShader, "stickRadius");
        float stickRadius = headRadius/2.0f;
        SetShaderValue(matchShader, stickRadiusUniformLocation, &stickRadius, SHADER_UNIFORM_FLOAT);

        unsigned int stickLengthUniformLocation = GetShaderLocation(matchShader, "stickLength");
        float tileHeight = tileRect.height;
        float stickLength = tileHeight - headRadius - outlineThickness*2.0f - stickRadius;
        SetShaderValue(matchShader, stickLengthUniformLocation, &stickLength, SHADER_UNIFORM_FLOAT);

        unsigned int outlineThicknessUniformLocation = GetShaderLocation(matchShader, "outlineThickness");
        SetShaderValue(matchShader, outlineThicknessUniformLocation, &outlineThickness, SHADER_UNIFORM_FLOAT);

        unsigned int burnProgressUniformLocation = GetShaderLocation(matchShader, "burnProgress");
        float burnTime = std::fmod(burnDelaySeconds - sumbitTimer.getSecondsRemaining(), burnDelaySeconds);
        if(burnTime == 0.0f){
            float invertedComputerBurnTime = std::min(computerTimer.getSecondsRemaining(), burnDelaySeconds);
            burnTime = std::fmod(burnDelaySeconds - invertedComputerBurnTime, burnDelaySeconds);
        }
        float burnProgress = burnTime/burnDelaySeconds;
        SetShaderValue(matchShader, burnProgressUniformLocation, &burnProgress, SHADER_UNIFORM_FLOAT); // for charing animation


        // selected fire
        unsigned int timeUniformLocation = GetShaderLocation(matchShader, "time");
        float time = GetTime();
        SetShaderValue(matchShader, timeUniformLocation, &time, SHADER_UNIFORM_FLOAT);

        unsigned int fireRadiusUniformLocation = GetShaderLocation(matchShader, "fireRadius");
        float fireRadius = headRadius*3.0f;
        SetShaderValue(matchShader, fireRadiusUniformLocation, &fireRadius, SHADER_UNIFORM_FLOAT);

        unsigned int fireColour1UniformLocation = GetShaderLocation(matchShader, "fireColour1");
        Vector4 fireColour1 = fireColour;
        SetShaderValue(matchShader, fireColour1UniformLocation, &fireColour1, SHADER_UNIFORM_VEC4);
        unsigned int fireColour2UniformLocation = GetShaderLocation(matchShader, "fireColour2");
        Vector4 fireColour2 = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        SetShaderValue(matchShader, fireColour2UniformLocation, &fireColour2, SHADER_UNIFORM_VEC4);

        
        unsigned i = 0;
        for(const auto& rowSelectionSet: gameTiles){
            for(ContainerBox* child: rowSelectionSet->getMembers()){
                if(auto* tile = dynamic_cast<GameTile*>(child)){
                    if(tile->getIsDisabled()) continue;
                    Rectangle rect = tile->getRect();
                    Vector2 headCentre = {rect.x + rect.width/2.0f, rect.y + headRadius + outlineThickness};
                    setHeadCentreUniform(matchShader, i, headCentre);
                    setIsMarkedUniform(matchShader, i, tile->getIsMarked());
                    ++i;
                }
            }
        }
        unsigned int matchCountUniformLocation = GetShaderLocation(matchShader, "matchCount");
        int matchCount = i;
        assert(matchCount <= MAX_TILES);
        SetShaderValue(matchShader, matchCountUniformLocation, &matchCount, SHADER_UNIFORM_INT);
        while(i<MAX_TILES){
            Vector2 headCentre = {0.0, 0.0};
            setHeadCentreUniform(matchShader, i, headCentre);
            ++i;
        }

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RED);
    EndShaderMode();
}

void GameBoard::update(){
    sumbitTimer.update();
    computerTimer.update();
}

void GameBoard::submit(){
    if(getIsPaused()) return;
    if(selectedRow == nullptr) return;
    sumbitTimer.setSecondsRemaining(burnDelaySeconds);
    sumbitTimer.resume(); // on end call sumbit function
}

void GameBoard::setSelectedRow(GameTileSelectionSet* row){
    if(row == selectedRow) return;
    if(selectedRow != nullptr){
        for(ContainerBox* child: selectedRow->getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                tile->unmark();
            }
        }
    }
    selectedRow = row;
}

void GameBoard::selectTiles(int rowIndex, unsigned count){
    int i = 0;
    auto rowSelectionSet = gameTiles[rowIndex]->getMembers();
    while(count > 0){
        if(auto* tile = dynamic_cast<GameTile*>(rowSelectionSet[i])) {
            if(!tile->getIsDisabled()){
                tile->mark();
                --count;
            }
            ++i;
        }
        else{
            std::terminate();
        }
    }
}

void GameBoard::computerMove(){
    rowSums.clear();
    unsigned nimSum = 0;
    unsigned singlesRemaining = 0;
    unsigned multisRemaining = 0;
    for(const auto& row: gameTiles){
        unsigned rowSum = 0;
        for(ContainerBox* child: row->getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                if(!tile->getIsDisabled()) rowSum += 1;
            }
        }
        rowSums.push_back(rowSum);
        if(rowSum > 1) ++multisRemaining;
        else if(rowSum == 1) ++singlesRemaining;
        nimSum ^= rowSum; // bitwise xor
    }
    
    unsigned msbMask = std::bit_floor(nimSum);
    unsigned rowIndex;
    unsigned removeCount;
    if(multisRemaining + singlesRemaining == 0){ // game is over
        return;
    }
    else if(nimSum != 0 or (multisRemaining == 0 and singlesRemaining%2 == 0)){ // computer wins
        if(multisRemaining > 1){
            rowIndex = getRowByMSB(msbMask);
            removeCount = rowSums[rowIndex] - (nimSum ^ rowSums[rowIndex]);
        }
        else if(multisRemaining == 1){ // entering endgame
            rowIndex = getNonSingleRow();
            // make the non-single row 1 or clear it in order to keep row count odd 
            if(singlesRemaining%2 == 0) removeCount = rowSums[rowIndex] - 1;
            else removeCount = rowSums[rowIndex];
        }
        else{ // forced moves in endgame
            rowIndex = getRowByMSB(1); // find singlerow 
            removeCount = 1;
        }
    }
    else if(singlesRemaining > 0 or multisRemaining > 0){ // computer loses, make random move
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rowDist(0, singlesRemaining + multisRemaining - 1);
        rowIndex = rowDist(gen);
        for(unsigned i = 0; i < rowSums.size(); ++i){
            if(rowSums[i] == 0) ++rowIndex;
            if(rowIndex == i) break;
        }
        std::uniform_int_distribution<> removeCountDist(1, rowSums[rowIndex]);
        removeCount = removeCountDist(gen);
    }
    selectTiles(rowIndex, removeCount);
}

void GameBoard::resetState(){
    for(const auto& rowSelectionSet: gameTiles){
        for(ContainerBox* child: rowSelectionSet->getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                tile->enable();
                tile->unmark();
            }
        }
        rowSelectionSet->resizeBoundingBox();
    }
}

bool GameBoard::getIsPaused(){
    return sumbitTimer.getIsCounting() or computerTimer.getIsCounting();
}
