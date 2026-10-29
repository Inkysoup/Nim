#include "gameTile.hpp"
#include "gameBoard.hpp"
#include "constants.hpp"


using namespace constants;

GameTile::GameTile(Vector2 position, float tileWidth, float tileHeight, SelectionSet* rowSet):
        Button(position, tileWidth, tileHeight, darkTanColour){
    isMarked = false;
    Button::bindOnClick([this, rowSet](){
        isMarked ? unmark() : mark();
        if(GameBoard* parent = dynamic_cast<GameBoard*>(getParent())){
            parent->setSelectedRow(rowSet);
        }
    });
}

GameTile::~GameTile(){
}

void GameTile::Draw(){
    //Button::Draw();
    if(isMarked) DrawRectangleRec(getRect(), Color{255, 255, 0, 128});
}

void GameTile::mark(){
    isMarked = true;
}
void GameTile::unmark(){
    isMarked = false;
}
bool GameTile::getIsMarked(){
    return isMarked;
}
