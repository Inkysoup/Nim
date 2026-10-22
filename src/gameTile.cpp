#include "gameTile.hpp"
#include "constants.hpp"

using namespace constants;

GameTile::GameTile(Vector2 position, float tileDiameter):
        Button(position, tileDiameter, tileDiameter, darkTanColour){
    Button::bindOnClick([this](){
        Button::disable();
    });
}

GameTile::~GameTile(){
}
