#include "Character.h"
#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;

void Character::make_sprite(float x, float y, float size)
{
    characterTexture.loadFromFile("data/images/character.png");
    characterTexture.setSmooth(true);
    characterSprite.setTexture(characterTexture);
    characterSprite.setPosition(x, y);
    characterSprite.setScale(size / characterTexture.getSize().x, size / characterTexture.getSize().x);
}

bool Character::can_move(std::vector <Sprite> borderSprites, int x, int y) {
    FloatRect pos = characterSprite.getGlobalBounds();
    int x1, x2, y1, y2;
    FloatRect block;
    for (Sprite elem : borderSprites) {
        block = elem.getGlobalBounds();
        x1 = block.left;
        x2 = block.left + block.width;
        y1 = block.top;
        y2 = block.top + block.height;

        // Левый верхний угол
        if (pos.left + x >= x1 && pos.left + x <= x2 && pos.top + y >= y1 && pos.top + y <= y2) {
            return false;
        }

        // Левый нижний угол
        if (pos.left + x >= x1 && pos.left + x <= x2 && pos.top + y + pos.height >= y1 && pos.top + y + pos.height <= y2) {
            return false;
        }

        // Правый верхний угол
        if (pos.left + x + pos.width >= x1 && pos.left + x + pos.width <= x2 && pos.top + y >= y1 && pos.top + y <= y2) {
            return false;
        }

        // Правый нижний угол
        if (pos.left + x + pos.width >= x1 && pos.left + x + pos.width <= x2 && pos.top + y + pos.height >= y1 && pos.top + y + pos.height <= y2) {
            return false;
        }
    }
    return true;
}