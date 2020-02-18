#include <SFML/Graphics.hpp>
#include <iostream>

#include "Utils/Animation.h"
#include "Utils/FPSCounter.h"
using namespace std;
sf::Vector2u WINDOW_SIZE{1400,900};
constexpr unsigned TPS = 60; //ticks per seconds
const sf::Time  timePerUpdate = sf::seconds(1.0f / float(TPS));
sf::Vector2i WORLD_SIZE{32,32};
sf::Vector2f TILE_SIZE{64.f,64.f};

enum class Dir{Down,Left,Up,Right,upLeft,upRight,downLeft,downRight};

struct Tile {
	sf::Sprite sprite;
	sf::Vector2i position;
};

void setFrameTime(std::vector<Animation>* v, float frameTime) {
	for(auto& it : *v) {
		it.setFrameTime(frameTime);
	}
}

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
	;
	auto speed = 100.f;
	bool lostFocus = false;

	sf::Font font;
	if(!font.loadFromFile("assets/arial.ttf"))
		std::cout<<"Failed to load arial.ttf\n";
	FPSCounter fpsCounter{};
	fpsCounter.setFont(&font);

	Animation upWalk;
	Animation downWalk;
	Animation leftWalk;
	Animation rightWalk;
	Animation upLeftWalk;
	Animation upRightWalk;
	Animation downLeftWalk;
	Animation downRightWalk;

	for (int i = 0; i < 6 ; i++)
     {
		downWalk.addFrame({i*128,0,128,192},0.1);
		leftWalk.addFrame({i*128,192*2,128,192},0.1);
		upWalk.addFrame({i*128,192*4,128,192},0.1);
		rightWalk.addFrame({i*128,192*6,128,192},0.1);
		
		downLeftWalk.addFrame({i*128,192*1,128,192},0.1);
		upLeftWalk.addFrame({i*128,192*3,128,192},0.1);
		upRightWalk.addFrame({i*128,192*5,128,192},0.1);
		downRightWalk.addFrame({i*128,192*7,128,192},0.1);	
	}

	std::vector<Animation> walkAnims{downWalk,leftWalk,upWalk,rightWalk,downLeftWalk,upLeftWalk, upRightWalk, downRightWalk};

	Animation upIdle;
	Animation downIdle;
	Animation leftIdle;
	Animation rightIdle;
	Animation upLeftIdle;
	Animation upRightIdle;
	Animation downLeftIdle;
	Animation downRightIdle;
	
	for (int i = 0; i < 6 ; i++)
     {
		downIdle.addFrame({i*128,0,128,192},0.1);
		leftIdle.addFrame({i*128,192*2,128,192},0.1);
		upIdle.addFrame({i*128,192*4,128,192},0.1);
		rightIdle.addFrame({i*128,192*6,128,192},0.1);
		
		downLeftIdle.addFrame({i*128,192*1,128,192},0.1);
		upLeftIdle.addFrame({i*128,192*3,128,192},0.1);
		upRightIdle.addFrame({i*128,192*5,128,192},0.1);
		downRightIdle.addFrame({i*128,192*7,128,192},0.1);	
	}

	sf::Texture texWalk;
	texWalk.loadFromFile("WizardWalkRight.png");
	sf::Texture texIdle;
	texIdle.loadFromFile("WizardIdleRight.png");
	
	sf::Sprite spr{texWalk};
	spr.setPosition((float)WINDOW_SIZE.x/2.f,(float)WINDOW_SIZE.y/2.f);
	spr.setTextureRect(downIdle.getFrame());

	std::vector<std::vector<Tile>> worldMap;

	sf::Texture texTiles;
	texTiles.loadFromFile("assets/tilea4.png");
	
	for(auto x = 0; x < WORLD_SIZE.x; x++) {
		worldMap.emplace_back();
		for(auto y = 0; y < WORLD_SIZE.y; y++) {
			auto tile = new Tile;
			tile->sprite.setTexture(texTiles);
			tile->sprite.setTextureRect({static_cast<int>(TILE_SIZE.x) * 6, 32,static_cast<int>(TILE_SIZE.x),static_cast<int>(TILE_SIZE.y)});
			tile->position = {x,y};
			worldMap[x].push_back(*tile);		
		}
	}

	
	auto dir = Dir::Down;
	
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

				if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
					speed=200;
					setFrameTime(&walkAnims,0.05f);
				}
				else {
					speed=100;
					setFrameTime(&walkAnims,0.1f);
				}
	

				//! ** INPUT SECTION **
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[5].getFrame());			
					spr.move(-speed * timePerUpdate.asSeconds(), -speed * timePerUpdate.asSeconds());
					dir = Dir::upLeft;
				}
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[6].getFrame());			
					spr.move(speed * timePerUpdate.asSeconds(), -speed * timePerUpdate.asSeconds());

					dir = Dir::upRight;
				}
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[4].getFrame());			
					spr.move(-speed * timePerUpdate.asSeconds(), speed * timePerUpdate.asSeconds());

					dir = Dir::downLeft;
				}
				else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[7].getFrame());			
					spr.move(speed * timePerUpdate.asSeconds(), speed * timePerUpdate.asSeconds());

					dir = Dir::downRight;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[2].getFrame());			
					spr.move(0.f, -speed * timePerUpdate.asSeconds());
					dir = Dir::Up;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[1].getFrame());
					spr.move(-speed * timePerUpdate.asSeconds(), 0.f);
					dir = Dir::Left;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[0].getFrame());
					spr.move(0.f, speed * timePerUpdate.asSeconds());
					dir = Dir::Down;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					spr.setTexture(texWalk);
					spr.setTextureRect(walkAnims[3].getFrame());
					spr.move(speed * timePerUpdate.asSeconds(), 0.f);
					dir = Dir::Right;
				}
				else {
					spr.setTexture(texIdle);
					
					if(dir == Dir::Down)
						spr.setTextureRect(downIdle.getFrame());
					else if(dir == Dir::Left)
						spr.setTextureRect(leftIdle.getFrame());
					else if(dir == Dir::Up)
						spr.setTextureRect(upIdle.getFrame());
					else if(dir == Dir::Right)
						spr.setTextureRect(rightIdle.getFrame());
					else if(dir == Dir::downLeft)
						spr.setTextureRect(downLeftIdle.getFrame());
					else if(dir == Dir::upLeft)
						spr.setTextureRect(upLeftIdle.getFrame());
					else if(dir == Dir::downRight)
						spr.setTextureRect(downRightIdle.getFrame());
					else if(dir == Dir::upRight)
						spr.setTextureRect(upRightIdle.getFrame());
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

			for(auto it : worldMap) {
				for(auto tile : it) {
					tile.sprite.setPosition(tile.position.x * TILE_SIZE.x,tile.position.y*TILE_SIZE.y);
					window.draw(tile.sprite);
				}
			}
			window.draw(spr);

			window.setView(window.getDefaultView());
			fpsCounter.draw(window);
			
			window.display();
		}
	
	return EXIT_SUCCESS;
}