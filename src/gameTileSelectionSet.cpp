#include "gameTileSelectionSet.hpp"
#include "gameTile.hpp"

#include <ranges>

GameTileSelectionSet::GameTileSelectionSet(){
}

GameTileSelectionSet::~GameTileSelectionSet(){
}

void GameTileSelectionSet::handleKeyboardInput(int keycode){
    if(keycode == KEY_ENTER or keycode == KEY_RIGHT){
        for(ContainerBox* child: getMembers()){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                if(!tile->getIsDisabled() and !tile->getIsMarked()){
                    tile->mark();
                    return;
                }
            }
        }
    }
    if(keycode == KEY_BACKSPACE or keycode == KEY_LEFT){
        for(ContainerBox* child: getMembers() | std::views::reverse){
            if(auto* tile = dynamic_cast<GameTile*>(child)) {
                if(!tile->getIsDisabled() and tile->getIsMarked()){
                    tile->unmark();
                    return;
                }
            }
        }
    }
}