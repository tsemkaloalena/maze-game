#ifndef __Character_H
#define __Character_H
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace sf;

class Character
{
public:
    std::string side = "right";
    Texture characterTexture1;
    Texture characterTexture2;
    Sprite characterSprite;
    Character() {};
    void make_sprite(std::string theme, float x, float y, float size);
    bool can_move(std::vector <Sprite> borderSprites, int x, int y);
    void turn(std::string direction);
};
#endif