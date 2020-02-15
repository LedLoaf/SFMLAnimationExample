#include <SFML/Graphics.hpp>
#include <iostream>

#include "Utils/Animation.h"
#include "Utils/FPSCounter.h"
using namespace std;
sf::Vector2u WINDOW_SIZE{1400,900};
constexpr unsigned TPS = 60; //ticks per seconds
const sf::Time  timePerUpdate = sf::seconds(1.0f / float(TPS));
sf::Vector2f TILE_SIZE{64.f,64.f};

enum Dir{Down,Left,Up,Right};

int main() {
	sf::RenderWindow window{sf::VideoMode{WINDOW_SIZE.x,WINDOW_SIZE.y},"SFML BoilerPlate"};
//	window.setFramerateLimit(60);
	window.setPosition(sf::Vector2i{window.getPosition().x,0});

	sf::Clock clock;
	auto lastTime = sf::Time::Zero;
    auto lag      = sf::Time::Zero;
	auto dt = 0.f;
	unsigned ticks = 0;
	auto view = window.getDefaultView();
	
	sf::CircleShape shape{TILE_SIZE.x/3.f};
	shape.setPosition((float)WINDOW_SIZE.x/2.f,(float)WINDOW_SIZE.y/2.f);
	auto speed = 100.f;
	bool lostFocus = false;

	sf::Font font;
	if(!font.loadFromFile("../../arial.ttf"))
		std::cout<<"Failed to load arial.ttf\n";
	FPSCounter fpsCounter{};
	fpsCounter.setFont(&font);

	Animation upWalk;
	Animation downWalk;
	Animation leftWalk;
	Animation rightWalk;


	for (int i = 0; i < 6 ; i++)
     {
		downWalk.addFrame({i*128,0,128,192},0.1);
		leftWalk.addFrame({i*128,192*2,128,192},0.1);
		upWalk.addFrame({i*128,192*4,128,192},0.1);
		rightWalk.addFrame({i*128,192*6,128,192},0.1);
	}

	Animation upIdle;
	Animation downIdle;
	Animation leftIdle;
	Animation rightIdle;
	for (int i = 0; i < 6 ; i++)
     {
		downIdle.addFrame({i*128,0,128,192},0.1);
		leftIdle.addFrame({i*128,192*2,128,192},0.1);
		upIdle.addFrame({i*128,192*4,128,192},0.1);
		rightIdle.addFrame({i*128,192*6,128,192},0.1);
	}

	sf::Texture texWalk;
	texWalk.loadFromFile("WizardWalkRight.png");

	sf::Texture texIdle;
	texIdle.loadFromFile("WizardIdleRight.png");
	
	sf::Sprite spr{texWalk};
	spr.setPosition((float)WINDOW_SIZE.x/2.f,(float)WINDOW_SIZE.y/2.f);
	spr.setTextureRect(downIdle.getFrame());

	Dir dir = Dir::Down;
	
	while (window.isOpen()) {

		auto time = clock.getElapsedTime();
        auto elapsed = time - lastTime;
        lastTime = time;
        lag += elapsed;
		
		fpsCounter.update();

		 //! Real time update
		  //Fixed time update
        while (lag >= timePerUpdate)
        {
            ticks++;
            lag -= timePerUpdate;
            //x fixedUpdate(elapsed);
        }

			sf::Event event{};
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
					window.close();
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
						case sf::Keyboard::Enter: cout << "Enter Pressed\n"; break;
						case sf::Keyboard::Space: cout << "Space Pressed\n"; break;
						default: break;
					}
				}
				if (event.type == sf::Event::Resized) {
					// update the view to the new size of the window
					view.setSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
				}
				if (event.type == sf::Event::LostFocus) { lostFocus = true; }
				if (event.type == sf::Event::GainedFocus) { lostFocus = false; }
			}

			if (!lostFocus) {
				auto mousePos = sf::Mouse::getPosition(window);
				auto mouseWorldPos = window.mapPixelToCoords(mousePos, view);

				window.setTitle("Mouse Position: (" + std::to_string(int(mouseWorldPos.x / 64.f)) + ", " +
					std::to_string(int(mouseWorldPos.y / 64.f)) + ")");

				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (mouseWorldPos.x >= 0 && mouseWorldPos.y >= 0 &&
						mouseWorldPos.x < WINDOW_SIZE.x && mouseWorldPos.y < WINDOW_SIZE.y) {
					}
				}

				//! ** INPUT SECTION **
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(upWalk.getFrame());			
					spr.move(0.f, -speed * timePerUpdate.asSeconds());
					dir = Dir::Up;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(leftWalk.getFrame());
					spr.move(-speed * timePerUpdate.asSeconds(), 0.f);
					dir = Dir::Left;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(downWalk.getFrame());
					spr.move(0.f, speed * timePerUpdate.asSeconds());
					dir = Dir::Down;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(rightWalk.getFrame());
					spr.move(speed * timePerUpdate.asSeconds(), 0.f);
					dir = Dir::Right;
				}
				else {
					spr.setTexture(texIdle);
					
					if(dir == Down)
						spr.setTextureRect(downIdle.getFrame());
					else 	if(dir == Left)
						spr.setTextureRect(leftIdle.getFrame());
					else 	if(dir == Up)
						spr.setTextureRect(upIdle.getFrame());
					else 	if(dir == Right)
						spr.setTextureRect(rightIdle.getFrame());
				}

				//! ** UPDATE SECTION**
				
				view.setCenter(spr.getPosition());
				window.setView(view);
			}

			//! ** DRAW SECTION **
			window.clear();

			for (auto x = 0; x < WINDOW_SIZE.x / TILE_SIZE.x; x++) {
				for (auto y = 0; y < WINDOW_SIZE.y / TILE_SIZE.y; y++) {
					sf::RectangleShape tile{{TILE_SIZE.x,TILE_SIZE.y}};
					tile.setFillColor({0,125,0,45});
					tile.setOutlineThickness(1.f);
					tile.setOutlineColor({sf::Color::Black});
					tile.setPosition(x * TILE_SIZE.x, y * TILE_SIZE.y);
					window.draw(tile);
				}
			}

			window.draw(shape);
			window.draw(spr);

			window.setView(window.getDefaultView());
			fpsCounter.draw(window);
			
			window.display();
		}
	
	return EXIT_SUCCESS;
}