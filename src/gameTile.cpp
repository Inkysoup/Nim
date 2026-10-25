#include "gameTile.hpp"
#include "constants.hpp"

using namespace constants;

GameTile::GameTile(Vector2 position, float tileWidth, float tileHeight, SelectionSet* rowSet):
        Button(position, tileWidth, tileHeight, darkTanColour){
    Button::bindOnClick([this, rowSet](){
        Button::disable();
        rowSet->resizeBoundingBox();
    });
}

GameTile::~GameTile(){
}
