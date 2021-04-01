#include "Character.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace sf;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 850;
int GAME_WIDTH = 800;
int GAME_HEIGHT = 800;
std::string theme;
// ����� ������ ���� ��������� ��� �������, ����������� � ���� ����� (�������, ����� main)
void load_level(int num);
void game_run();

std::vector <std::vector <char>> level_map; // ��������� ������ ��� �������� ������ ������
// # - �������; . - ������; @ - ������, ��������� ��������� ���������
std::vector <std::vector <Texture>> fieldTextures;
std::vector <Sprite> borderSprites;
std::vector <Sprite> roadSprites;

Character character;

int main() {
    game_run();
    return 0;
}


void load_level(int num) {
    // ������� ��������. �� ������, ����� ������� �� ������.
    borderSprites.clear();
    roadSprites.clear();
    fieldTextures.clear();
    borderSprites.shrink_to_fit();
    roadSprites.shrink_to_fit();
    fieldTextures.shrink_to_fit();

    // ���������� �� ���������� �����, ������ ���� ���������� "<����� ������>.txt"
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
    data.close();

    // ������ ������� � vector
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

    // ������ �������� � 2 ������� (������� � ������)
    float block_size = (float)GAME_WIDTH / level_map[0].size();
    for (int i = 0; i < level_map.size(); i++) {
        for (int j = 0; j < level_map[i].size(); j++) {
            Sprite fieldSprite;
            fieldSprite.setTexture(fieldTextures[i][j]);
            fieldSprite.setPosition(i * block_size, j * block_size + WINDOW_HEIGHT - GAME_HEIGHT);
            fieldSprite.setScale(block_size / fieldTextures[i][j].getSize().x, block_size / fieldTextures[i][j].getSize().x);

            if (level_map[i][j] == '.') {
                roadSprites.push_back(fieldSprite);
            }
            else if (level_map[i][j] == '#') {
                borderSprites.push_back(fieldSprite);
            }
            else if (level_map[i][j] == '@') {
                roadSprites.push_back(fieldSprite);
                character.make_sprite(j * block_size, i * block_size + WINDOW_HEIGHT - GAME_HEIGHT, block_size * 0.7);
            }
        }
    }
}


// ������� � ��������� ����
void game_run()
{
    float SPEED = 5.0;
    int FRAME_NUMBER = 0;
    int TIMER = 0;
    bool SCORE_RECORDED = false;
    bool PLAY = true;
    
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Maze");
    window.setFramerateLimit(30);

    load_level(1);

    Font font;
    font.loadFromFile("./data/fonts/HotMustardBTNPosterRegular.ttf");
    Text timerText("", font, 30);
    timerText.setFillColor(Color(0, 0, 0));
    timerText.setPosition(10, 10);

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
            if (character.characterSprite.getPosition().y > WINDOW_HEIGHT - GAME_HEIGHT) {
                if (character.can_move(borderSprites, 0, -SPEED)) {
                    character.characterSprite.move(0, -SPEED);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
            if (character.characterSprite.getPosition().y + character.characterSprite.getGlobalBounds().height < WINDOW_HEIGHT) {
                if (character.can_move(borderSprites, 0, SPEED)) {
                    character.characterSprite.move(0, SPEED);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            if (character.characterSprite.getPosition().x > 0) {
                if (character.can_move(borderSprites, -SPEED, 0)) {
                    character.characterSprite.move(-SPEED, 0);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            if (character.characterSprite.getPosition().x + character.characterSprite.getGlobalBounds().width < GAME_WIDTH) {
                if (character.can_move(borderSprites, SPEED, 0)) {
                    character.characterSprite.move(SPEED, 0);
                }
            }
        }

        window.clear(Color(218, 255, 88));

        for (Sprite elem : borderSprites) {
            window.draw(elem);
        }
        for (Sprite elem : roadSprites) {
            window.draw(elem);
        }
        window.draw(character.characterSprite);

        FRAME_NUMBER++;
        if (FRAME_NUMBER % 30 == 0) {
            TIMER++;
        }
        std::stringstream ss;
        ss << TIMER;
        timerText.setString(ss.str());
        window.draw(timerText);

        window.display();
    }
}