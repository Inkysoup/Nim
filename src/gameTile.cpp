#include "gameTile.hpp"
#include "gameBoard.hpp"
#include "constants.hpp"


using namespace constants;

GameTile::GameTile(Vector2 position, float tileWidth, float tileHeight, GameTileSelectionSet* rowSet):
        Button(position, tileWidth, tileHeight, darkTanColour){
    setAllowedActive(false);
    setAutoDraw(false);
    isMarked = false;
    this->rowSet = rowSet;
    Button::bindOnClick([this, rowSet](){
        if(GameBoard* parent = dynamic_cast<GameBoard*>(getParent())){
            if(parent->getIsPaused()) return;
        }
        isMarked ? unmark() : mark();
    });
}

GameTile::~GameTile(){
}

void GameTile::drawMarked(){
    if(isMarked) DrawRectangleRec(getRect(), Color{255, 255, 0, 128});
}

void GameTile::mark(){
    if(GameBoard* parent = dynamic_cast<GameBoard*>(getParent())){
        if(parent->getIsPaused()) return;
        parent->setSelectedRow(rowSet);
    }
    isMarked = true;
}
void GameTile::unmark(){
    if(GameBoard* parent = dynamic_cast<GameBoard*>(getParent())){
        if(parent->getIsPaused()) return;
    }
    isMarked = false;
}
bool GameTile::getIsMarked(){
    return isMarked;
}
