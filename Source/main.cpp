#include <iostream>
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include "imgui-SFML.h"
#include "imgui.h"


struct SnakeSegment
{
	int x, y;
	SnakeSegment(int _x, int _y) : x(_x), y(_y) {}
};

std::vector<SnakeSegment> snake; 
int direction = 0; 
sf::RectangleShape food; 

bool isGameOver = false;

void HandleUserInput(sf::RenderWindow &window, const sf::Event &event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		window.close();
		break;
	case sf::Event::KeyPressed:
		if (!isGameOver)
		{
			if (event.key.code == sf::Keyboard::Up && direction != 2)
				direction = 0;
			else if (event.key.code == sf::Keyboard::Right &&
					 direction != 3) 
				direction = 1;
			else if (event.key.code == sf::Keyboard::Down && direction != 0) 
				direction = 2;
			else if (event.key.code == sf::Keyboard::Left && direction != 1) 
				direction = 3;
		}
		break;
	default:
		break;
	}
}

void Update(sf::RenderWindow &window, const sf::Time &deltaClock)
{
	if (!isGameOver)
	{
		// Обновление положения змеи и проверка на столкновение
		int headX = snake[0].x;
		int headY = snake[0].y;
		if (direction == 0)
			headY--;
		else if (direction == 1)
			headX++;
		else if (direction == 2)
			headY++;
		else if (direction == 3)
			headX--;

		// Проверка на столкновение с границами
		if (headX < 0 || headX >= 800 || headY < 0 || headY >= 600)
		{			
			isGameOver = true;
			return;
		}

		// Проверка на столкновение с змеей
		for (size_t i = 1; i < snake.size(); i++)
		{
			if (headX == snake[i].x && headY == snake[i].y)
			{
				isGameOver = true;
				return;
			}
		}

		// Проверка на столкновение с едой
		if (headX < food.getPosition().x + food.getSize().x && headX + 30 > food.getPosition().x &&
			headY < food.getPosition().y + food.getSize().y && headY + 30 > food.getPosition().y)
		{
			for (int i = 0; i < 14; i++) 
			{
				snake.push_back(SnakeSegment(headX, headY));
			}
			int foodX, foodY;
			do
			{
				foodX = (rand() % 40) * 20;
				foodY = (rand() % 40) * 20;
			}
			while (foodX >= 800 || foodY >= 500); 

			food.setPosition(foodX, foodY);
		}

		snake.pop_back();
		snake.insert(snake.begin(), SnakeSegment(headX, headY));
	}
}


void Render(sf::RenderWindow &window)
{
	for (const SnakeSegment &segment : snake)
	{
		sf::RectangleShape snakeSegment(sf::Vector2f(30, 30));
		snakeSegment.setPosition(segment.x, segment.y);
		snakeSegment.setFillColor(sf::Color::Green); 
		window.draw(snakeSegment);
	}
	window.draw(food);

	if (isGameOver)
	{
		ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Once);
		ImGui::Begin("Game Over", &isGameOver, ImGuiWindowFlags_NoResize);
		ImGui::Text("Game Over!");
		if (ImGui::Button("Play Again"))
		{
			snake.clear();
			snake.push_back(SnakeSegment(400, 400));
			direction = 0;
			isGameOver = false;
		}
		ImGui::End();
	}
}

void RenderGui(sf::RenderWindow &window)
{
	ImGui::Begin("Snake Game");
	ImGui::Text("Welcome to Snake Game!");
	ImGui::Text("Use arrow keys to control the snake.");
	ImGui::End();
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Snake Game");
	window.setFramerateLimit(70); 
	if (!ImGui::SFML::Init(window))
	{
		std::cout << "ImGui initialization failed\n";
		return -1;
	}
	snake.push_back(SnakeSegment(400, 400));

	int foodX = (rand() % 40) * 20; 
	int foodY = (rand() % 40) * 20;

	food.setSize(sf::Vector2f(15, 15)); 
	food.setPosition(foodX, foodY);
	food.setFillColor(sf::Color::Red);


	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);
			HandleUserInput(window, event);
		}

		sf::Time deltaTime = deltaClock.restart();
		ImGui::SFML::Update(window, deltaTime);
		Update(window, deltaTime);

		window.clear();

		RenderGui(window);
		Render(window);

		ImGui::SFML::Render(window);

		window.display();
	}
	ImGui::SFML::Shutdown();

	return 0;
}
