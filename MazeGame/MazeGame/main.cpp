#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace sf;

int WIDTH = 800;
int HEIGHT = 800;
std::string theme;
// Здесь должны быть объявлены все функции, находящиеся в этом файле (конечно, кроме main)
void load_level(int num);
void add_character(float x, float y, float size);
void game_run();

std::vector <std::vector <char>> level_map; // Двумерный массив для хранения знаков уровня
// # - граница; . - дорога; @ - дорога, начальное положение персонажа
std::vector <std::vector <Texture>> fieldTextures;
std::vector <Sprite> borderSprites;
std::vector <Sprite> roadSprites;
// Текстура и спрайт персонажа
Texture characterTexture;
Sprite character;

int main() {
    game_run();
    return 0;
}


void load_level(int num) {
    // Очистка векторов. На случай, когда уровень не первый.
    borderSprites.clear();
    roadSprites.clear();
    fieldTextures.clear();
    borderSprites.shrink_to_fit();
    roadSprites.shrink_to_fit();
    fieldTextures.shrink_to_fit();

    // Считывание из текстового файла, причём файл называется "<номер уровня>.txt"
    std::string number = std::to_string(num);
    std::string filename = "data/levels/" + number + ".txt";
    std::fstream data;
    data.open(filename, std::ios::in);
    getline(data, theme);
    int i = 0;
    std::string line;
    while (getline(data, line)) {
        level_map.push_back(std::vector<char>());
        for (int j = 0; j < line.size(); j++) {
            level_map[i].push_back(line.c_str()[j]);
        }
        i++;
    }

    // Запись текстур в vector
    std::string type;
    for (int i = 0; i < level_map.size(); i++) {
        fieldTextures.push_back(std::vector<Texture>());
        for (int j = 0; j < level_map[i].size(); j++) {
            Texture fieldTexture;
            if (level_map[i][j] == '.' or level_map[i][j] == '@') {
                type = "road";
            }
            else if (level_map[i][j] == '#') {
                type = "border";
            }
            fieldTexture.loadFromFile("data/images/" + theme + "_" + type + ".jpg");
            fieldTextures[i].push_back(fieldTexture);
        }
    }

    // Запись спрайтов в 2 вектора (границы и дорога)
    float block_size = (float)WIDTH / level_map[0].size();
    for (int i = 0; i < level_map.size(); i++) {
        for (int j = 0; j < level_map[i].size(); j++) {
            Sprite fieldSprite;
            fieldSprite.setTexture(fieldTextures[i][j]);
            fieldSprite.setPosition(i * block_size, j * block_size);
            fieldSprite.setScale(block_size / fieldTextures[i][j].getSize().x, block_size / fieldTextures[i][j].getSize().x);

            if (level_map[i][j] == '.') {
                roadSprites.push_back(fieldSprite);
            }
            else if (level_map[i][j] == '#') {
                borderSprites.push_back(fieldSprite);
            }
            else if (level_map[i][j] == '@') {
                roadSprites.push_back(fieldSprite);
                add_character(j * block_size, i * block_size, block_size * 0.7);
            }
        }
    }
}

void add_character(float x, float y, float size) {
    characterTexture.loadFromFile("data/images/character.png");
    characterTexture.setSmooth(true);
    character.setTexture(characterTexture);
    character.setPosition(x, y);
    character.setScale(size / characterTexture.getSize().x, size / characterTexture.getSize().x);
}

bool can_move(FloatRect character, int x, int y) {
    int x1, x2, y1, y2;
    FloatRect block;
    for (Sprite elem : borderSprites) {
        block = elem.getGlobalBounds();
        x1 = block.left;
        x2 = block.left + block.width;
        y1 = block.top;
        y2 = block.top + block.height;

        // Левый верхний угол
        if (character.left + x >= x1 && character.left + x <= x2 && character.top + y >= y1 && character.top + y <= y2) {
            return false;
        }

        // Левый нижний угол
        if (character.left + x >= x1 && character.left + x <= x2 && character.top + y + character.height >= y1 && character.top + y + character.height <= y2) {
            return false;
        }

        // Правый верхний угол
        if (character.left + x + character.width >= x1 && character.left + x + character.width <= x2 && character.top + y >= y1 && character.top + y <= y2) {
            return false;
        }

        // Правый нижний угол
        if (character.left + x + character.width >= x1 && character.left + x + character.width <= x2 && character.top + y + character.height >= y1 && character.top + y + character.height <= y2) {
            return false;
        }
    }
    return true;
}

void game_run()
{
    float SPEED = 5.0;
    int FRAME_NUMBER = 0;
    int SCORE = 0;
    bool SCORE_RECORDED = false;
    bool PLAY = true;
    
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Maze");
    window.setFramerateLimit(30);

    load_level(1);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case Event::Closed:
                window.close();
                break;

            
            }

        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
            if (character.getPosition().y > 0) {
                if (can_move(character.getGlobalBounds(), 0, -SPEED)) {
                    character.move(0, -SPEED);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
            if (character.getPosition().y < HEIGHT) {
                if (can_move(character.getGlobalBounds(), 0, SPEED)) {
                    character.move(0, SPEED);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            if (character.getPosition().y < HEIGHT) {
                if (can_move(character.getGlobalBounds(), -SPEED, 0)) {
                    character.move(-SPEED, 0);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            if (character.getPosition().y < HEIGHT) {
                if (can_move(character.getGlobalBounds(), SPEED, 0)) {
                    character.move(SPEED, 0);
                }
            }
        }

        window.clear();
        for (Sprite elem : borderSprites) {
            window.draw(elem);
        }
        for (Sprite elem : roadSprites) {
            window.draw(elem);
        }
        window.draw(character);
        window.display();
    }
}