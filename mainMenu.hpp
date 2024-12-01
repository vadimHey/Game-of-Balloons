#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
using namespace sf;

#define maxMainMenu 5

void setText(Text &text, Font &font, Color color, string description, int size, float width, float height)
{
	text.setFont(font);
	text.setFillColor(color);
	text.setString(description);
	text.setCharacterSize(size);
	text.setPosition(width, height / maxMainMenu);
}

class MainMenu
{
public:
	MainMenu(float width, float height);
	void draw(RenderWindow& window);
	void moveUp();
	void moveDown();
	int mainMenuPressed() {
		return mainMenuSelected;
	}
	~MainMenu();
private:
	int mainMenuSelected;
	Font font;
	Text mainMenu[maxMainMenu];
};

MainMenu::MainMenu(float width, float height) 
{
	if (!font.loadFromFile("involved/font.ttf"))
		cout << "No font is here" << endl;

	//Играть
	setText(mainMenu[0], font, Color::Black, "Play game", 60, width / 2.0, height * 1.2);
	//Цель игры
	setText(mainMenu[1], font, Color::Black, "Purpose of game", 60, width / 2.0, height * 1.7);
	//Справка об управлении
	setText(mainMenu[2], font, Color::Black, "Management info", 60, width / 2.0, height * 2.2);
	//Сведения об авторе
	setText(mainMenu[3], font, Color::Black, "Author's data", 60, width / 2.0, height * 2.7);
	//Выход из игры
	setText(mainMenu[4], font, Color::Black, "Exit", 60, width / 2.0, height * 3.2);

	mainMenuSelected = -1;
}

MainMenu::~MainMenu() {}

//Вывод меню 
void MainMenu::draw(RenderWindow& window) 
{
	for (int i = 0; i < maxMainMenu; ++i) 
	{
		window.draw(mainMenu[i]);
	}
}

//Движение вверх
void MainMenu::moveUp()
{
	if (mainMenuSelected - 1 >= -1) 
	{
		mainMenu[mainMenuSelected].setFillColor(Color::Black);
		mainMenuSelected--;
		if (mainMenuSelected == -1)
			mainMenuSelected = maxMainMenu - 1;
		mainMenu[mainMenuSelected].setFillColor(Color::Red);
	}
}

//Движение вниз
void MainMenu::moveDown()
{
	if (mainMenuSelected + 1 <= maxMainMenu)
	{
		mainMenu[mainMenuSelected].setFillColor(Color::Black);
		mainMenuSelected++;
		if (mainMenuSelected == maxMainMenu)
			mainMenuSelected = 0;
		mainMenu[mainMenuSelected].setFillColor(Color::Red);
	}
}
