#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
using namespace sf;
void Menu(RenderWindow& window)
{
	Texture MenuTextureStart, MenuTextureExit;
	MenuTextureStart.loadFromFile("Start.png");
	MenuTextureExit.loadFromFile("Exit.png");
	Sprite MenuStart(MenuTextureStart), MenuExit(MenuTextureExit);
	bool isMenu = 1;
	int MenuNum = 0;
	MenuStart.setPosition(100, 30);
	MenuExit.setPosition(100, 90);
	while (isMenu)
	{
		MenuStart.setColor(Color::White);
		MenuExit.setColor(Color::White);
		MenuNum = 0;
		window.clear(Color(129, 181, 221));
		if (IntRect(100, 30, 300, 50).contains(Mouse::getPosition(window))) { MenuStart.setColor(Color::Blue); MenuNum = 1; }
		if (IntRect(100, 90, 300, 50).contains(Mouse::getPosition(window))) { MenuExit.setColor(Color::Blue); MenuNum = 2; }
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (MenuNum == 1) isMenu = false;
			if (MenuNum == 2) { window.close(); isMenu = false; }
		}
		window.draw(MenuStart);
		window.draw(MenuExit);
		window.display();
	}
}
int main() 
{
	setlocale(0, "");
	RenderWindow window(VideoMode(350, 150), "SFML works!");
	Menu(window);
	return 0;
}