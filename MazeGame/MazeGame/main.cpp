#include "Character.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace sf;

int SPACE_HEIGHT = 50;
int WIDTH = 600;
int HEIGHT = 600;
int SCORE = 0;
std::string theme;
// Здесь должны быть объявлены все функции, находящиеся в этом файле (конечно, кроме main)
void bonus_generate();
void load_level(int num);
void game_run();
void Menu();

std::vector <std::vector <char>> level_map; // Двумерный массив для хранения знаков уровня
// # - граница; . - дорога; @ - дорога, начальное положение персонажа
std::vector <std::vector <int>> bonus_map;
std::vector <std::vector <int>> no_bonus_YX;
std::vector <std::vector <Texture>> fieldTextures;
std::vector <Sprite> borderSprites;
std::vector <Sprite> roadSprites;

std::string* bonusImages = new std::string[4];
std::vector <Texture> bonusTextures;
std::vector <Sprite> bonusSprites;
std::string* stuffImages = new std::string[3];
std::vector <Texture> stuffTextures;
std::vector <Sprite> stuffSprites;
Texture finishTexture;
Sprite finishSprite;


Character character;

int main() {
    //game_run();
    Menu();
    return 0;
}


void bonus_generate() {
    float block_size = (float)HEIGHT / level_map[0].size();
    srand(time(NULL));
    int t = rand() % bonusTextures.size();
    int num = rand() % no_bonus_YX.size();
    int bonus_X = no_bonus_YX[num][1];
    int bonus_Y = no_bonus_YX[num][0];
    Sprite bonusSprite;
    bonusSprite.setTexture(bonusTextures[t]);
    bonusSprite.setPosition(bonus_X * block_size + block_size * 0.225, bonus_Y * block_size + SPACE_HEIGHT + block_size * 0.225);
    bonusSprite.setScale(block_size / bonusTextures[t].getSize().x * 0.55, block_size / bonusTextures[t].getSize().x * 0.55);
    bonusSprites.push_back(bonusSprite);
    bonus_map[bonus_Y][bonus_X] = 1;
    no_bonus_YX.erase(no_bonus_YX.begin() + num);
}


void load_level(int num) {
    // Очистка векторов. На случай, когда уровень не первый.
    borderSprites.clear();
    roadSprites.clear();
    fieldTextures.clear();
    borderSprites.shrink_to_fit();
    roadSprites.shrink_to_fit();
    fieldTextures.shrink_to_fit();

    finishTexture.loadFromFile("data/images/finish.png");
    finishSprite.setTexture(finishTexture);

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
        bonus_map.push_back(std::vector<int>());
        for (int j = 0; j < line.size(); j++) {
            level_map[i].push_back(line.c_str()[j]);
            bonus_map[i].push_back(0);
        }
        i++;
    }
    data.close();

    bonusImages[0] = "data/images/" + theme + "_bonus1.png";
    bonusImages[1] = "data/images/" + theme + "_bonus2.png";
    bonusImages[2] = "data/images/coin1.png";
    bonusImages[3] = "data/images/coin2.png";

    stuffImages[0] = "data/images/" + theme + "_stuff1.png";
    stuffImages[1] = "data/images/" + theme + "_stuff2.png";
    stuffImages[2] = "data/images/" + theme + "_stuff3.png";

    // Запись текстур в vector
    std::string type;
    for (int i = 0; i < level_map.size(); i++) {
        fieldTextures.push_back(std::vector<Texture>());
        for (int j = 0; j < level_map[i].size(); j++) {
            Texture fieldTexture;
            if (level_map[i][j] == '.' or level_map[i][j] == '@' or level_map[i][j] == 'f') {
                type = "road";
                if ((i + j) % 5 == rand() % 5) {
                    Texture bonusTexture;
                    bonusTexture.loadFromFile(bonusImages[rand() % 4]);
                    bonusTextures.push_back(bonusTexture);
                }
            }
            else if (level_map[i][j] == '#') {
                type = "border";
                if ((i + j) % 7 == 3) {
                    Texture stuffTexture;
                    stuffTexture.loadFromFile(stuffImages[rand() % 3]);
                    stuffTextures.push_back(stuffTexture);
                }
            }
            fieldTexture.loadFromFile("data/images/" + theme + "_" + type + ".jpg");
            fieldTextures[i].push_back(fieldTexture);
        }
    }

    // Запись спрайтов в 2 вектора (границы и дорога)
    int k = 0;
    float block_size = (float)HEIGHT / level_map[0].size();
    for (int i = 0; i < level_map.size(); i++) {
        for (int j = 0; j < level_map[i].size(); j++) {
            Sprite fieldSprite;
            fieldSprite.setTexture(fieldTextures[i][j]);
            fieldSprite.setPosition(j * block_size, i * block_size + SPACE_HEIGHT);
            fieldSprite.setScale(block_size / fieldTextures[i][j].getSize().x, block_size / fieldTextures[i][j].getSize().x);

            if (level_map[i][j] == '.') {
                roadSprites.push_back(fieldSprite);
            }
            else if (level_map[i][j] == '#') {
                borderSprites.push_back(fieldSprite);
                if ((i + j) % 5 == rand() % 5) {
                    Sprite stuffSprite;
                    stuffSprite.setTexture(stuffTextures[k]);
                    stuffSprite.setPosition(j * block_size, i * block_size + SPACE_HEIGHT - block_size * 0.3);
                    stuffSprite.setScale(block_size / stuffTextures[k].getSize().x, block_size / stuffTextures[k].getSize().x);
                    stuffSprites.push_back(stuffSprite);
                    k++;
                }
                bonus_map[i][j] = 2;
            }
            else if (level_map[i][j] == '@') {
                roadSprites.push_back(fieldSprite);
                character.make_sprite(theme, j * block_size, i * block_size + SPACE_HEIGHT, block_size * 0.8);
                bonus_map[i][j] = 2;
            }
            else if (level_map[i][j] == 'f') {
                roadSprites.push_back(fieldSprite);
                finishSprite.setScale(block_size / finishTexture.getSize().x, block_size / finishTexture.getSize().y);
                finishSprite.setPosition(j * block_size, i * block_size + SPACE_HEIGHT);
                bonus_map[i][j] = 2;
            }
        }
    }
    int cnt_no_bonus = 0;
    for (int i = 0; i < bonus_map.size(); i++) {
        for (int j = 0; j < bonus_map[0].size(); j++) {
            if (bonus_map[i][j] == 0) {
                no_bonus_YX.push_back({ i, j });
                cnt_no_bonus++;
            }
        }
    }
    int bonus_cnt = 0.3 * roadSprites.size();
    while (bonus_cnt > 0) {
        bonus_generate();
        bonus_cnt--;
    }
}


void Menu()
{
    RenderWindow window(VideoMode(350, 300), "Maze game - menu");
    Font font;
    font.loadFromFile("./data/fonts/Roboto-Regular.ttf");
    Text start("Start", font, 70);
    start.setFillColor(Color(0, 7, 77));
    Text score("Score", font, 70);
    score.setFillColor(Color(0, 7, 77));
    Text exit("Exit", font, 70);
    exit.setFillColor(Color(0, 7, 77));
    start.setPosition(100, 30);
    score.setPosition(100, 90);
    exit.setPosition(100, 150);
    bool isMenu = 1;
    int MenuNum = 0;

    //while (window.isOpen())

    while (isMenu)
    {
        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case Event::Closed:
                window.close();
                break;
            }
        }

        MenuNum = 0;
        window.clear(Color(129, 181, 221));
        if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { start.setFillColor(Color::Blue); MenuNum = 1; }
        else { start.setFillColor(Color(0, 7, 77)); }
        if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) { score.setFillColor(Color::Blue); MenuNum = 2; }
        else { score.setFillColor(Color(0, 7, 77)); }
        if (IntRect(100, 150, 300, 50).contains(Mouse::getPosition(window))) { exit.setFillColor(Color::Blue); MenuNum = 3; }
        else { exit.setFillColor(Color(0, 7, 77)); }
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            if (MenuNum == 1) { window.close(); isMenu = false; game_run(); }
            if (MenuNum == 2) isMenu = false;
            if (MenuNum == 3) { window.close(); isMenu = false; }
        }
        window.draw(start);
        window.draw(score);
        window.draw(exit);
        window.display();
    }
}


// Функция с процессом игры
void game_run()
{
    float SPEED = 5.0;
    int FRAME_NUMBER = 0;
    int TIMER = 0;
    bool SCORE_RECORDED = false;
    bool PLAY = true;
    HEIGHT = VideoMode::getDesktopMode().height - SPACE_HEIGHT * 4;
    WIDTH = HEIGHT;

    RenderWindow window(VideoMode(WIDTH, HEIGHT + SPACE_HEIGHT), "Maze game");
    window.setPosition(sf::Vector2i(0, 0));
    window.setFramerateLimit(30);

    load_level(1);

    Font font;
    font.loadFromFile("./data/fonts/HotMustardBTNPosterRegular.ttf");
    Text timerText("", font, 30);
    timerText.setFillColor(Color(0, 0, 0));
    timerText.setPosition(10, 10);
    Text scoreText1("", font, 30);
    scoreText1.setFillColor(Color(0, 0, 0));
    scoreText1.setPosition(WIDTH - 180, 10);
    Text scoreText2("", font, 30);
    scoreText2.setFillColor(Color(0, 0, 0));
    scoreText2.setPosition(WIDTH - 65, 10);

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
            if (character.characterSprite.getGlobalBounds().top > SPACE_HEIGHT) {
                if (character.can_move(borderSprites, 0, -SPEED)) {
                    character.characterSprite.move(0, -SPEED);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
            if (character.characterSprite.getGlobalBounds().top + character.characterSprite.getGlobalBounds().height < HEIGHT + SPACE_HEIGHT) {
                if (character.can_move(borderSprites, 0, SPEED)) {
                    character.characterSprite.move(0, SPEED);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            if (character.characterSprite.getGlobalBounds().left > 0) {
                character.turn("left");
                if (character.can_move(borderSprites, -SPEED, 0)) {
                    character.characterSprite.move(-SPEED, 0);
                }
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            if (character.characterSprite.getGlobalBounds().left + character.characterSprite.getGlobalBounds().width < WIDTH) {
                if (character.can_move(borderSprites, SPEED, 0)) {
                    character.characterSprite.move(SPEED, 0);
                    character.turn("right");
                }
            }
        }

        //bonus
        float block_size = (float)HEIGHT / level_map[0].size();
        float сharacter_X = character.characterSprite.getPosition().x;
        float сharacter_Y = character.characterSprite.getPosition().y;
        int i = сharacter_Y / block_size;
        int j = сharacter_X / block_size;
        if (bonus_map[i][j] == 1) {
            SCORE++;
            for (int e = 0; e < bonusSprites.size(); e++) {
                Sprite elem = bonusSprites[e];
                float bonus_X = j * block_size + block_size * 0.225;
                float bonus_Y = i * block_size + block_size * 0.225 + SPACE_HEIGHT;
                if (elem.getPosition().x == bonus_X && elem.getPosition().y == bonus_Y) {
                    bonusSprites.erase(bonusSprites.begin() + e);
                    break;
                }
            }
            bonus_map[i][j] = 0;
            no_bonus_YX.push_back({ i, j });
            bonus_generate();
        }
        

        window.clear(Color(218, 255, 88));

        for (Sprite elem : borderSprites) {
            window.draw(elem);
        }
        for (Sprite elem : roadSprites) {
            window.draw(elem);
        }
        for (Sprite elem : bonusSprites) {
            window.draw(elem);
        }
        if (FRAME_NUMBER % 15 < 7) {
            character.characterSprite.setTexture(character.characterTexture1);
        }
        else {
            character.characterSprite.setTexture(character.characterTexture2);
        }
        window.draw(character.characterSprite);

        for (Sprite elem : stuffSprites) {
            window.draw(elem);
        }
        window.draw(finishSprite);

        FRAME_NUMBER++;
        if (FRAME_NUMBER % 30 == 0) {
            TIMER++;
        }
        std::stringstream ss;
        ss << TIMER;
        timerText.setString(ss.str());
        window.draw(timerText);
        scoreText1.setString("Score");
        window.draw(scoreText1);
        std::stringstream bonus_cnt;
        bonus_cnt << SCORE;
        scoreText2.setString(bonus_cnt.str());
        window.draw(scoreText2);

        window.display();
    }
}