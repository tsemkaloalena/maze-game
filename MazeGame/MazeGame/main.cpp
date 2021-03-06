#include "Character.h"
#include "Enemy.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include  <io.h>
using namespace sf;

int LEVEL = 1;
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
bool level_exists(int n);
void save_number_of_level(int n);
void load_number_of_level();
bool CursorButtonCheck(int x, int y, Text Object);
std::string getScoreList(int& start, int k);
void scorelist_view();

std::vector <std::vector <char>> level_map; // Двумерный массив для хранения знаков уровня
// # - граница; . - дорога; @ - дорога, начальное положение персонажа
std::vector <std::vector <int>> bonus_map;
std::vector <std::vector <int>> no_bonus_YX;
std::vector <std::vector <Texture>> fieldTextures;
std::vector <Sprite> borderSprites;
std::vector <Sprite> roadSprites;

std::string* bonusImages = new std::string[4];
std::vector <Texture> bonusTextures;
std::vector <Sprite> bonusSprites1;
std::vector <Sprite> bonusSprites2;
std::string* stuffImages = new std::string[3];
std::vector <Texture> stuffTextures;
std::vector <Sprite> stuffSprites;
Texture finishTexture;
Sprite finishSprite;

Character character;

std::vector<Enemy> horizontalEnemies;
std::vector<Enemy> verticalEnemies;

int main() {
	//game_run();
	Menu();
	return 0;
}


void bonus_generate() {
	float block_size = (float)HEIGHT / level_map[0].size();
	srand(time(NULL));
	int num = rand() % no_bonus_YX.size();
	int bonus_X = no_bonus_YX[num][1];
	int bonus_Y = no_bonus_YX[num][0];
	int t = (bonus_X + bonus_Y) % 2;
	Sprite bonusSprite;
	bonusSprite.setTexture(bonusTextures[t]);
	bonusSprite.setPosition(bonus_X * block_size + block_size * 0.225, bonus_Y * block_size + SPACE_HEIGHT + block_size * 0.225);
	bonusSprite.setScale(block_size / bonusTextures[t].getSize().x * 0.55, block_size / bonusTextures[t].getSize().x * 0.55);
	if (t == 0) {
		bonusSprites1.push_back(bonusSprite);
	}
	else {
		bonusSprites2.push_back(bonusSprite);
	}
	bonus_map[bonus_Y][bonus_X] = 1;
	no_bonus_YX.erase(no_bonus_YX.begin() + num);
}

bool CursorButtonCheck(int x, int y, Text Object) {
	if (x > Object.getPosition().x and x < Object.getPosition().x + Object.getLocalBounds().width) {
		if (y > Object.getPosition().y and y < Object.getPosition().y + Object.getLocalBounds().height) {
			return true;
		}
	}
	return false;
}

bool level_exists(int n) {
	std::stringstream ss;
	ss << n;
	std::string path = "data/levels/" + ss.str() + ".txt";
	if (_access(path.c_str(), 0) != -1) {
		return true;
	}
	return false;
}

void save_number_of_level(int n) {
	std::ofstream fout("data/levels/current.txt", std::ios::out);
	fout << LEVEL;
	fout.close();
}
void load_number_of_level() {
	std::fstream data;
	data.open("data/levels/current.txt", std::ios::in);
	std::string level_str;
	getline(data, level_str);
	LEVEL = std::stoi(level_str);

	data.close();
}

void load_level(int num) {
	// Очистка векторов. На случай, когда уровень не первый.
	bonusImages = new std::string[4];
	stuffImages = new std::string[3];

	bonus_map.clear();
	no_bonus_YX.clear();
	bonusTextures.clear();
	bonusSprites1.clear();
	bonusSprites2.clear();
	horizontalEnemies.clear();
	verticalEnemies.clear();
	level_map.clear();
	borderSprites.clear();
	roadSprites.clear();
	stuffSprites.clear();
	stuffTextures.clear();
	fieldTextures.clear();
	level_map.shrink_to_fit();
	borderSprites.shrink_to_fit();
	roadSprites.shrink_to_fit();
	fieldTextures.shrink_to_fit();
	stuffTextures.shrink_to_fit();
	stuffSprites.shrink_to_fit();
	bonus_map.shrink_to_fit();
	no_bonus_YX.shrink_to_fit();
	bonusTextures.shrink_to_fit();
	bonusSprites1.shrink_to_fit();
	bonusSprites2.shrink_to_fit();
	horizontalEnemies.shrink_to_fit();
	verticalEnemies.shrink_to_fit();

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
	int cnt = 0;
	std::string type;
	for (int i = 0; i < level_map.size(); i++) {
		fieldTextures.push_back(std::vector<Texture>());
		for (int j = 0; j < level_map[i].size(); j++) {
			Texture fieldTexture;
			if (level_map[i][j] == '.' or level_map[i][j] == '@' or level_map[i][j] == 'f' or level_map[i][j] == 'v' or level_map[i][j] == 'h') {
				type = "road";
				if (cnt < 4) {
					Texture bonusTexture;
					bonusTexture.loadFromFile(bonusImages[cnt]);
					bonusTexture.setSmooth(true);
					bonusTextures.push_back(bonusTexture);
					cnt++;
				}
			}
			else if (level_map[i][j] == '#') {
				type = "border";
				if ((i + j) % 5 == rand() % 5) {
					level_map[i][j] = 's';
					Texture stuffTexture;
					stuffTexture.loadFromFile(stuffImages[rand() % 3]);
					stuffTextures.push_back(stuffTexture);
				}
			}
			if (level_map[i][j] == 'h') {
				Enemy enemy;
				horizontalEnemies.push_back(enemy);
			}
			else if (level_map[i][j] == 'v') {
				Enemy enemy;
				verticalEnemies.push_back(enemy);
			}

			fieldTexture.loadFromFile("data/images/" + theme + "_" + type + ".jpg");
			fieldTextures[i].push_back(fieldTexture);
		}
	}

	// Запись спрайтов в 2 вектора (границы и дорога)
	int k = 0, vmonst = 0, hmonst = 0;
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
				bonus_map[i][j] = 2;
			}
			else if (level_map[i][j] == 's') {
				borderSprites.push_back(fieldSprite);
				Sprite stuffSprite;
				stuffSprite.setTexture(stuffTextures[k]);
				stuffSprite.setPosition(j * block_size, i * block_size + SPACE_HEIGHT - block_size * 0.3);
				stuffSprite.setScale(block_size / stuffTextures[k].getSize().x, block_size / stuffTextures[k].getSize().x);
				stuffSprites.push_back(stuffSprite);
				k++;
				bonus_map[i][j] = 2;
			}
			else if (level_map[i][j] == '@') {
				roadSprites.push_back(fieldSprite);
				character.make_sprite(theme, j * block_size, i * block_size + SPACE_HEIGHT, block_size * 0.8);
				bonus_map[i][j] = 2;
			}
			else if (level_map[i][j] == 'v') {
				roadSprites.push_back(fieldSprite);
				verticalEnemies[vmonst].make_sprite("up", theme, j * block_size + 0.1 * block_size, i * block_size + 0.1 * block_size + SPACE_HEIGHT, block_size * 0.8);
				vmonst++;

			}
			else if (level_map[i][j] == 'h') {
				roadSprites.push_back(fieldSprite);
				horizontalEnemies[hmonst].make_sprite("right", theme, j * block_size + 0.1 * block_size, i * block_size + 0.1 * block_size + SPACE_HEIGHT, block_size * 0.8);
				hmonst++;
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

std::string getScoreList(int& start, int k) {
	int place = 1;
	std::vector<std::string> text;
	std::string str_text = "";
	int i = 0;
	std::string line;
	std::fstream data;
	data.open("./data/scorelist.txt", std::ios::in);
	while (getline(data, line)) {
		std::string place_str = std::to_string(place);
		std::string line_temp = place_str + " - " + line + " points";
		text.push_back(line_temp);
		place++;
	}
	if (text.size() < k) {
		k = text.size();
	}
	if (text.size() - start < k) {
		start = text.size() - k;
	}
	for (int i = start; i < k + start; i++) {
		str_text += text[i] + "\n";
	}
	return str_text;
}

void scorelist_view() {
	HEIGHT = VideoMode::getDesktopMode().height - SPACE_HEIGHT * 4;
	WIDTH = HEIGHT;
	RenderWindow window(VideoMode(WIDTH, HEIGHT + SPACE_HEIGHT), "Maze game - score list");
	window.setPosition(sf::Vector2i(0, 0));
	window.setFramerateLimit(30);
	float SPEED = 5.0;
	int SCROLL_START = 0;

	RectangleShape rectangle(Vector2f(window.getSize().x * 0.8, window.getSize().y * 0.7));
	rectangle.move((window.getSize().x - rectangle.getLocalBounds().width) / 2, (window.getSize().y - rectangle.getLocalBounds().height) / 2);
	rectangle.setFillColor(Color(255, 255, 255, 50));

	Font font;
	font.loadFromFile("./data/fonts/HotMustardBTNPosterRegular.ttf");
	Text mainMenuBtn("Main menu", font, 50);
	mainMenuBtn.setFillColor(Color(0, 7, 77));
	mainMenuBtn.setPosition(rectangle.getPosition().x + (rectangle.getLocalBounds().width - mainMenuBtn.getLocalBounds().width) - 20, rectangle.getPosition().y + 10);

	Font title_list_font;
	title_list_font.loadFromFile("./data/fonts/LithosProBold.otf");
	Text list_title("Score list", title_list_font, 30);
	list_title.setFillColor(Color(0, 7, 77));

	Font list_font;
	list_font.loadFromFile("./data/fonts/LithosProRegular.ttf");
	Text list("", list_font, 30);
	list.setFillColor(Color(0, 7, 77));
	list.setString(getScoreList(SCROLL_START, 15));

	list_title.setPosition(rectangle.getPosition().x + 50, rectangle.getPosition().y + (rectangle.getLocalBounds().height - list.getLocalBounds().height) / 2 - 20);
	list.setPosition(rectangle.getPosition().x + 50, list_title.getPosition().y + list_title.getLocalBounds().height + 20);


	while (window.isOpen())
	{
		Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				void (*funct) ();
				funct = Menu;
				funct();
				break;
			case Event::KeyPressed:
				break;
			case Event::MouseMoved:
				if (CursorButtonCheck(event.mouseMove.x, event.mouseMove.y, mainMenuBtn)) {
					mainMenuBtn.setFillColor(Color(255, 255, 255));
				}
				else {
					mainMenuBtn.setFillColor(Color(0, 7, 77));
				}
				break;
			case Event::MouseButtonPressed:
				if (CursorButtonCheck(event.mouseButton.x, event.mouseButton.y, mainMenuBtn)) {
					window.close();
					void (*func) ();
					func = Menu;
					func();
				}
				break;
			case Event::MouseWheelScrolled:
				SCROLL_START -= event.mouseWheelScroll.delta;
				if (SCROLL_START < 0) {
					SCROLL_START = 0;
				}
				list.setString(getScoreList(SCROLL_START, 15));
			}

		}
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
		{
			window.close();
		}

		window.clear(Color(129, 181, 221));

		window.draw(rectangle);
		window.draw(mainMenuBtn);
		window.draw(list_title);
		window.draw(list);

		window.display();
	}
}

void Menu()
{
	RenderWindow window(VideoMode(400, 400), "Maze game - menu");
	Font font;
	font.loadFromFile("./data/fonts/HotMustardBTNPosterRegular.ttf");
	Text start("Start", font, 70);
	start.setFillColor(Color(0, 7, 77));
	Text score("Score", font, 70);
	score.setFillColor(Color(0, 7, 77));
	Text exit("Exit", font, 70);
	exit.setFillColor(Color(0, 7, 77));

	score.setPosition((window.getSize().x - score.getLocalBounds().width) / 2, (window.getSize().y - score.getLocalBounds().height) / 2);
	start.setPosition((window.getSize().x - start.getLocalBounds().width) / 2, score.getPosition().y - 90);
	exit.setPosition((window.getSize().x - exit.getLocalBounds().width) / 2, score.getPosition().y + 90);
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
		if (IntRect(start.getGlobalBounds()).contains(Mouse::getPosition(window))) { start.setFillColor(Color::Blue); MenuNum = 1; }
		else { start.setFillColor(Color(0, 7, 77)); }
		if (IntRect(score.getGlobalBounds()).contains(Mouse::getPosition(window))) { score.setFillColor(Color::Blue); MenuNum = 2; }
		else { score.setFillColor(Color(0, 7, 77)); }
		if (IntRect(exit.getGlobalBounds()).contains(Mouse::getPosition(window))) { exit.setFillColor(Color::Blue); MenuNum = 3; }
		else { exit.setFillColor(Color(0, 7, 77)); }
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (MenuNum == 1) { window.close(); isMenu = false; game_run(); }
			if (MenuNum == 2) { window.close(); isMenu = false; scorelist_view(); };
			if (MenuNum == 3) { window.close(); isMenu = false; }
		}
		window.draw(start);
		window.draw(score);
		window.draw(exit);
		window.display();
	}
}

SoundBuffer bonusBuffer;
Sound music_bonus(bonusBuffer);
SoundBuffer enemyBuffer;
Sound music_enemy(enemyBuffer);
SoundBuffer winBuffer;
Sound music_win(winBuffer);
SoundBuffer game_overBuffer;
Sound music_game_over(game_overBuffer);
void play_music(bool ic, std::string type, bool played=false)
{
	if (type == "bonus") {
		if (ic) {
			music_bonus.play();
		}
	}
	else if (type == "enemy") {
		if (ic) {
			music_enemy.play();
		}
	}
	else if (type == "win" && played==false) {
		if (ic) {
			music_win.play();
		}
	}
	else if (type == "game_over" && played==false) {
		if (ic) {
			music_game_over.play();
		}
	}
}

// Функция с процессом игры
void game_run()
{
	Music music;
	music.openFromFile("data/music/music.ogg");
	music.setVolume(5);
	music.play();
	music.setLoop(true);
	bool win_played = false;
	bool game_over_played = false;
	bonusBuffer.loadFromFile("data/music/bonus.ogg");
	enemyBuffer.loadFromFile("data/music/enemy.ogg");
	winBuffer.loadFromFile("data/music/win.ogg");
	game_overBuffer.loadFromFile("data/music/game_over.ogg");
	int life = 3;
	std::vector <bool> v_intersected;
	std::vector <bool> h_intersected;
	float SPEED = 5.0;
	int FRAME_NUMBER = 0;
	int TIMER = 0;
	bool SCORE_RECORDED = false;
	bool PLAY = true;
	bool WIN = false;
	HEIGHT = VideoMode::getDesktopMode().height - SPACE_HEIGHT * 4;
	WIDTH = HEIGHT;

	RenderWindow window(VideoMode(WIDTH, HEIGHT + SPACE_HEIGHT), "Maze game");
	window.setPosition(sf::Vector2i(0, 0));
	window.setFramerateLimit(30);

	load_number_of_level();
	load_level(LEVEL);

	for (Enemy elem : verticalEnemies) {
		v_intersected.push_back(false);
	}
	for (Enemy elem : horizontalEnemies) {
		h_intersected.push_back(false);
	}

	Font font;
	font.loadFromFile("./data/fonts/HotMustardBTNPosterRegular.ttf");
	Text timerText("", font, 30);
	Text lifeText("", font, 30);
	timerText.setFillColor(Color(0, 0, 0));
	lifeText.setFillColor(Color(0, 0, 0));
	timerText.setPosition(10, 10);
	lifeText.setPosition(200, 10);
	Text scoreText1("", font, 30);
	scoreText1.setFillColor(Color(0, 0, 0));
	scoreText1.setPosition(WIDTH - 180, 10);
	Text scoreText2("", font, 30);
	scoreText2.setFillColor(Color(0, 0, 0));
	scoreText2.setPosition(WIDTH - 65, 10);

	Text restartBtn("Restart playing", font, 60);
	restartBtn.setFillColor(Color(0, 0, 0));
	Text nextBtn("Next level", font, 60);
	nextBtn.setFillColor(Color(0, 0, 0, 100));
	Text scorelistBtn("Score list", font, 60);
	scorelistBtn.setFillColor(Color(0, 0, 0));
	Text exitBtn("Exit", font, 60);
	exitBtn.setFillColor(Color(0, 0, 0));
	int button_space = 30;
	int button_y = restartBtn.getLocalBounds().height + nextBtn.getLocalBounds().height + scorelistBtn.getLocalBounds().height + exitBtn.getLocalBounds().height + button_space * 3;
	nextBtn.setPosition((window.getSize().x - nextBtn.getLocalBounds().width) / 2, (window.getSize().y - button_y) / 2);
	restartBtn.setPosition((window.getSize().x - restartBtn.getLocalBounds().width) / 2, nextBtn.getPosition().y + nextBtn.getLocalBounds().height + button_space);
	scorelistBtn.setPosition((window.getSize().x - scorelistBtn.getLocalBounds().width) / 2, restartBtn.getPosition().y + restartBtn.getLocalBounds().height + button_space);
	exitBtn.setPosition((window.getSize().x - exitBtn.getLocalBounds().width) / 2, scorelistBtn.getPosition().y + scorelistBtn.getLocalBounds().height + button_space);
	RectangleShape rectangle(Vector2f(window.getSize().x * 0.8, button_y + 150));
	rectangle.move((window.getSize().x - rectangle.getLocalBounds().width) / 2, (window.getSize().y - rectangle.getLocalBounds().height) / 2);
	rectangle.setFillColor(Color(218, 255, 95, 200));


	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case Event::Closed:
				window.close();
				break;

			case Event::KeyPressed:
				break;
			case Event::MouseMoved:
				if (WIN && level_exists(LEVEL + 1)) {
					if (CursorButtonCheck(event.mouseMove.x, event.mouseMove.y, nextBtn)) {
						nextBtn.setFillColor(Color(255, 255, 255));
					}
					else {
						nextBtn.setFillColor(Color(0, 0, 0));
					}
				}
				else {
					nextBtn.setFillColor(Color(0, 0, 0, 100));
				}
				if (CursorButtonCheck(event.mouseMove.x, event.mouseMove.y, restartBtn)) {
					restartBtn.setFillColor(Color(255, 255, 255));
				}
				else {
					restartBtn.setFillColor(Color(0, 0, 0));
				}
				if (CursorButtonCheck(event.mouseMove.x, event.mouseMove.y, scorelistBtn)) {
					scorelistBtn.setFillColor(Color(255, 255, 255));
				}
				else {
					scorelistBtn.setFillColor(Color(0, 0, 0));
				}
				if (CursorButtonCheck(event.mouseMove.x, event.mouseMove.y, exitBtn)) {
					exitBtn.setFillColor(Color(255, 255, 255));
				}
				else {
					exitBtn.setFillColor(Color(0, 0, 0));
				}
				break;
			case Event::MouseButtonPressed:
				if (not PLAY) {
					if (WIN && CursorButtonCheck(event.mouseButton.x, event.mouseButton.y, nextBtn)) {
						if (level_exists(LEVEL + 1)) {
							LEVEL++;
							save_number_of_level(LEVEL);
							window.close();
							void (*func) ();
							func = game_run;
							func();
						}
					}
					else if (CursorButtonCheck(event.mouseButton.x, event.mouseButton.y, restartBtn)) {
						window.close();
						void (*func) ();
						func = game_run;
						func();
					}
					else if (CursorButtonCheck(event.mouseButton.x, event.mouseButton.y, scorelistBtn)) {
						window.close();
						void (*func) ();
						func = scorelist_view;
						func();
					}
					else if (CursorButtonCheck(event.mouseButton.x, event.mouseButton.y, exitBtn)) {
						window.close();
					}
					break;
				}
			}
		}
		if (PLAY) {
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
			if (character.side == "left") {
				сharacter_X -= 0.45 * block_size;
			}
			else {
				сharacter_X += 0.45 * block_size;
			}
			float сharacter_Y = character.characterSprite.getPosition().y - SPACE_HEIGHT + 0.4 * block_size;
			int i = сharacter_Y / block_size;
			int j = сharacter_X / block_size;
			if (bonus_map[i][j] == 1) {
				SCORE++;
				play_music(true, "bonus");
				for (int e = 0; e < bonusSprites1.size(); e++) {
					Sprite elem = bonusSprites1[e];
					float bonus_X = j * block_size + block_size * 0.225;
					float bonus_Y = i * block_size + block_size * 0.225 + SPACE_HEIGHT;
					if (elem.getPosition().x == bonus_X && elem.getPosition().y == bonus_Y) {
						bonusSprites1.erase(bonusSprites1.begin() + e);
						break;
					}
				}
				for (int e = 0; e < bonusSprites2.size(); e++) {
					Sprite elem = bonusSprites2[e];
					float bonus_X = j * block_size + block_size * 0.225;
					float bonus_Y = i * block_size + block_size * 0.225 + SPACE_HEIGHT;
					if (elem.getPosition().x == bonus_X && elem.getPosition().y == bonus_Y) {
						bonusSprites2.erase(bonusSprites2.begin() + e);
						break;
					}
				}
				bonus_map[i][j] = 0;
				no_bonus_YX.push_back({ i, j });
				bonus_generate();
			}

			int old_life = life;
			for (int i = 0; i < horizontalEnemies.size(); i++) {
				horizontalEnemies[i].enemy_move(borderSprites, SPEED, SPACE_HEIGHT, HEIGHT, WIDTH);
				if (!h_intersected[i] && character.characterSprite.getGlobalBounds().intersects(horizontalEnemies[i].enemySprite.getGlobalBounds())) {
					life--;
					h_intersected[i] = true;
				}
				else if (!(character.characterSprite.getGlobalBounds().intersects(horizontalEnemies[i].enemySprite.getGlobalBounds()))) {
					h_intersected[i] = false;
				}
			}
			for (int i = 0; i < verticalEnemies.size(); i++) {
				verticalEnemies[i].enemy_move(borderSprites, SPEED, SPACE_HEIGHT, HEIGHT, WIDTH);
				if (!v_intersected[i] && character.characterSprite.getGlobalBounds().intersects(verticalEnemies[i].enemySprite.getGlobalBounds())) {
					life--;
					v_intersected[i] = true;
				}
				else if (!(character.characterSprite.getGlobalBounds().intersects(verticalEnemies[i].enemySprite.getGlobalBounds()))) {
					v_intersected[i] = false;
				}
			}
			if (life < old_life) {
				play_music(true, "enemy");
			}
		}

		if (character.characterSprite.getGlobalBounds().intersects(finishSprite.getGlobalBounds())) {
			WIN = true;
			music.pause();
			PLAY = false;
		}

		if (life <= 0)
		{
			music.pause();
			PLAY = false;
		}


		window.clear(Color(218, 255, 88));

		for (Sprite elem : borderSprites) {
			window.draw(elem);
		}
		for (Sprite elem : roadSprites) {
			window.draw(elem);
		}
		for (Sprite elem : bonusSprites1) {
			//int num = rand() % 2;
			if (FRAME_NUMBER % 20 < 10) {
				elem.setTexture(bonusTextures[0]);
			}
			else {
				elem.setTexture(bonusTextures[1]);
			}
			window.draw(elem);
		}
		for (Sprite elem : bonusSprites2) {
			//int num = rand() % 2;
			if (FRAME_NUMBER % 20 < 10) {
				elem.setTexture(bonusTextures[2]);
			}
			else {
				elem.setTexture(bonusTextures[3]);
			}
			window.draw(elem);
		}
		if (FRAME_NUMBER % 15 < 7) {
			character.characterSprite.setTexture(character.characterTexture1);
		}
		else {
			character.characterSprite.setTexture(character.characterTexture2);
		}
		window.draw(character.characterSprite);

		if (FRAME_NUMBER % 15 < 7) {
			for (int i = 0; i < horizontalEnemies.size(); i++) {
				horizontalEnemies[i].enemySprite.setTexture(horizontalEnemies[i].enemyTexture1);
			}
			for (int i = 0; i < verticalEnemies.size(); i++) {
				verticalEnemies[i].enemySprite.setTexture(verticalEnemies[i].enemyTexture1);
			}
		}
		else {
			for (int i = 0; i < horizontalEnemies.size(); i++) {
				horizontalEnemies[i].enemySprite.setTexture(horizontalEnemies[i].enemyTexture2);
			}
			for (int i = 0; i < verticalEnemies.size(); i++) {
				verticalEnemies[i].enemySprite.setTexture(verticalEnemies[i].enemyTexture2);
			}
		}

		for (Enemy elem : horizontalEnemies) {
			window.draw(elem.enemySprite);
		}
		for (Enemy elem : verticalEnemies) {
			window.draw(elem.enemySprite);
		}

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
		timerText.setString("time: " + ss.str());
		window.draw(timerText);
		scoreText1.setString("score: ");
		window.draw(scoreText1);
		std::stringstream bonus_cnt;
		bonus_cnt << SCORE;
		scoreText2.setString(bonus_cnt.str());
		window.draw(scoreText2);
		std::stringstream sss;
		sss << life;
		lifeText.setString("lives: " + sss.str());
		window.draw(lifeText);

		if (not PLAY) {
			if (WIN) {
				play_music(true, "win", win_played);
				win_played = true;
				if (not SCORE_RECORDED) {
					SCORE_RECORDED = true;

					//std::vector <std::vector <int>> scorelist;
					bool checker = false;
					int points;
					//int index = 0;
					std::fstream data1;
					std::ofstream data2;
					data1.open("./data/scorelist.txt", std::ios::in);
					data2.open("./data/new_scorelist.txt", std::ios::out);
					std::string line;
					while (getline(data1, line)) {
						points = atoi(line.c_str());
						//scorelist.push_back({ points });
						if (points > SCORE) {
							data2 << points << "\n";
						}
						else if (points < SCORE && !checker) {
							data2 << SCORE << "\n";
							data2 << points << "\n";
							checker = true;
						}
						else {
							data2 << points << "\n";
						}
					}
					data1.close();
					data2.close();
					remove("./data/scorelist.txt");
					rename("./data/new_scorelist.txt", "./data/scorelist.txt");
				}
				SCORE = 0;
			}
			else {
				play_music(true, "game_over", game_over_played);
				game_over_played = true;
			}
			window.draw(rectangle);
			window.draw(nextBtn);
			window.draw(restartBtn);
			window.draw(scorelistBtn);
			window.draw(exitBtn);
		}

		window.display();
	}
}