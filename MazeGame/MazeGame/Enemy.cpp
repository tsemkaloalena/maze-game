#include "Enemy.h"
#include <SFML/Graphics.hpp>
#include <vector>
using namespace sf;

void Enemy::make_sprite(std::string direction, std::string theme, float x, float y, float size)
{
    this->direction = direction;
    enemyTexture1.loadFromFile("data/images/winter_monster1.png");
    //characterTexture1.setSmooth(true);
    enemyTexture2.loadFromFile("data/images/winter_monster2.png");
    //characterTexture2.setSmooth(true);
    enemySprite.setTexture(enemyTexture1);
    enemySprite.setPosition(x, y);
    enemySprite.setScale(size / enemyTexture1.getSize().y, size / enemyTexture1.getSize().y);
}

bool Enemy::can_move(std::vector <Sprite> borderSprites, int x, int y) {
    FloatRect pos = enemySprite.getGlobalBounds();
    int x1, x2, y1, y2;
    FloatRect block;
    for (Sprite elem : borderSprites) {
        block = elem.getGlobalBounds();
        x1 = block.left;
        x2 = block.left + block.width;
        y1 = block.top;
        y2 = block.top + block.height;

        // Левый верхний угол
        if (pos.left + x >= x1 && pos.left + x <= x2 && pos.top + y >= y1 && pos.top + y <= y2 - block.height * 0.2) {
            return false;
        }

        // Левый нижний угол
        if (pos.left + x >= x1 && pos.left + x <= x2 && pos.top + y + pos.height >= y1 && pos.top + y + pos.height <= y2) {
            return false;
        }

        // Правый верхний угол
        if (pos.left + x + pos.width >= x1 && pos.left + x + pos.width <= x2 && pos.top + y >= y1 && pos.top + y <= y2 - block.height * 0.2) {
            return false;
        }

        // Правый нижний угол
        if (pos.left + x + pos.width >= x1 && pos.left + x + pos.width <= x2 && pos.top + y + pos.height >= y1 && pos.top + y + pos.height <= y2) {
            return false;
        }
    }
    return true;
}

void Enemy::turn(std::string direction) {
    Vector2f scale = enemySprite.getScale();
    enemySprite.setScale(-scale.x, scale.y);
    if (direction == "right") {
        enemySprite.move(-enemySprite.getGlobalBounds().width, 0);
    }
    else {
        enemySprite.move(enemySprite.getGlobalBounds().width, 0);
    }
}

void Enemy::enemy_move(std::vector <Sprite> borderSprites, int speed, int window_space_height, int window_height, int window_width) {
    if (direction == "up") {
        if (enemySprite.getGlobalBounds().top > window_space_height) {
            if (can_move(borderSprites, 0, -speed)) {
                enemySprite.move(0, -speed);
            }
            else {
                direction = "down";
            }
        }
        else {
            direction = "down";
        }
    }
    else if (direction == "down") {
        if (enemySprite.getGlobalBounds().top + enemySprite.getGlobalBounds().height + speed < window_height + window_space_height) {
            if (can_move(borderSprites, 0, speed)) {
                enemySprite.move(0, speed);
            }
            else {
                direction = "up";
            }
        }
        else {
            direction = "up";
        }
    }
    else if (direction == "left") {
        if (enemySprite.getGlobalBounds().left > 0) {
            if (can_move(borderSprites, -speed, 0)) {
                enemySprite.move(-speed, 0);
            }
            else {
                turn("right");
                direction = "right";
            }
        }
        else {
            turn("right");
            direction = "right";
        }
    }
    else if (direction == "right") {
        if (enemySprite.getGlobalBounds().left + enemySprite.getGlobalBounds().width < window_width) {
            if (can_move(borderSprites, speed, 0)) {
                enemySprite.move(speed, 0);
            }
            else {
                turn("left");
                direction = "left";
            }
        }
        else {
            turn("left");
            direction = "left";
        }
    }
}