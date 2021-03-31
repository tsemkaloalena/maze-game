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
void load_level(int num);
void game_run();

std::vector <Sprite> borderSprites;
std::vector <Sprite> roadSprites;
std::vector <std::vector <Texture>> fieldTextures;

int main() {
    game_run();
    return 0;
}

std::vector<std::vector<char>> level_map;
void load_level(int num) {
    borderSprites.clear();
    roadSprites.clear();
    fieldTextures.clear();
    borderSprites.shrink_to_fit();
    roadSprites.shrink_to_fit();
    fieldTextures.shrink_to_fit();

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
            }
        }
    }
}

void game_run()
{
    float SPEED = 10.0;
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

        window.clear();
        for (Sprite elem : borderSprites) {
            window.draw(elem);
        }
        for (Sprite elem : roadSprites) {
            window.draw(elem);
        }
        window.display();
    }
}