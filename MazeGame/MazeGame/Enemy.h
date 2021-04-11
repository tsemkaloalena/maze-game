#ifndef __Enemy_H
#define __Enemy_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace sf; 

class Enemy {
public:
    //std::string side = "right";
    std::string direction;
    Texture enemyTexture1;
    Texture enemyTexture2;
    Sprite enemySprite;
    Enemy() {};
    void make_sprite(std::string direction, std::string theme, float x, float y, float size);
    bool can_move(std::vector <Sprite> borderSprites, int x, int y);
    void turn(std::string direction);
    void enemy_move(std::vector <Sprite> borderSprites, int speed, int window_space_height, int window_height, int window_width);
};

#endif