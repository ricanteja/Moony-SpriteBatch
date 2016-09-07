#ifndef TESTMOONY_H
#define TESTMOONY_H

#include <vector>
#include <random>

#include <SFML/Graphics.hpp>
#include "Log.h"
#include "TextureAtlas.h"
#include "SpriteBatch.h"



int main()
{
	int count = 24000;

	// Random number generator
	std::random_device random;
	std::srand(random());

	// Create the window 
	sf::RenderWindow window(sf::VideoMode(800, 600), "MoonyTest", sf::Style::Close);

	sf::Clock frame_clock;
	sf::Time frame_limit = sf::seconds(1.0f / 60.0f);
	sf::Time frame_sum;
	sf::Time frame_time;
	int frame_count;

	sf::Clock clock;
	bool fullscreen = false;

	moony::Sprite player1;
	moony::Sprite player2;
	moony::TextureAtlas texture_atlas;
	moony::SpriteBatch sprite_batch;

	std::vector<moony::Sprite> sprites;

	if(!texture_atlas.loadFromFile("A/small/small.mtpf"))
		return -1;

	player1.m_subtexture = texture_atlas.findSubTexture("mario.png");
	player1.m_layer = 1;
	player1.setScale(5.0f, 5.0f);

	player2.m_subtexture = texture_atlas.findSubTexture("goomba.png");
	player2.m_layer = 4;
	player2.setScale(5.0f, 5.0f);

	std::vector<std::string> tex_names = texture_atlas.getSubTextureNames();

	for(size_t index = 0; index < count; index++)
	{
		size_t name_index = std::rand() % tex_names.size();
		moony::Texture sub_texture = texture_atlas.findSubTexture(tex_names[name_index]);
		//moony::Texture sub_texture = texture_atlas.findSubTexture("player.png");
		moony::Sprite sprite(sub_texture, std::rand() % 100);

		sprite.setPosition(std::rand() % 800, std::rand() % 600);
		sprite.setOrigin(sprite.m_subtexture.m_rect.width / 2, sprite.m_subtexture.m_rect.height / 2);
		sprites.push_back(sprite);
	}

	while(window.isOpen())
	{
		frame_time = frame_clock.restart();

		sf::Event event;

		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed: window.close(); break;
				default: break;
			}
		}

		frame_sum += frame_time;
		frame_count = 0;

		while(frame_sum > frame_limit && frame_count < 10)
		{
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
			{
				window.create(sf::VideoMode(800, 600), "MoonyTest", fullscreen ? sf::Style::Fullscreen : sf::Style::Close);
				fullscreen = !fullscreen;
			}
			
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add) && sprites.size() < 128000)
			{
				for(size_t index = 0; index < 100; index++)
				{
					size_t name_index = std::rand() % tex_names.size();
					moony::Texture sub_texture = texture_atlas.findSubTexture(tex_names[name_index]);
					//moony::Texture sub_texture = texture_atlas.findSubTexture("player.png");
					moony::Sprite sprite(sub_texture, 1);

					sprite.setPosition(std::rand() % 800, std::rand() % 600);
					sprite.setOrigin(sprite.m_subtexture.m_rect.width / 2, sprite.m_subtexture.m_rect.height / 2);
					sprites.push_back(sprite);
				}
			}

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) && sprites.size() > 0)
			{
				for(size_t index = 0; index < 100; index++)
					sprites.pop_back();
			}

			float cos = std::cos(clock.getElapsedTime().asSeconds());
			float sin = std::sin(clock.getElapsedTime().asSeconds());

			for(moony::Sprite& sprite : sprites)
			{
				sprite.rotate(cos * sprite.m_layer);
				sprite.move(cos, sin * sprite.m_layer * 0.25);
				sprite.setScale(sin * 2, sin * 2);
				//sprite.m_layer = random() % 10;
			}
			
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				player1.move(0, -5);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				player1.move(0, 5);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				player1.move(-5, 0);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				player1.move(5, 0);

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
				player1.m_subtexture = texture_atlas.findSubTexture("mario.png");
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::H))
				player1.m_subtexture = texture_atlas.findSubTexture("mushroomman.png");
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::N))
				player1.m_subtexture = texture_atlas.findSubTexture("cactus.png");

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				player2.move(0, -5);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				player2.move(0, 5);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				player2.move(-5, 0);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				player2.move(5, 0);

			if(sf::Keyboard::isKeyPressed(sf::Keyboard::T))
				player2.m_subtexture = texture_atlas.findSubTexture("player.png");
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::G))
				player2.m_subtexture = texture_atlas.findSubTexture("snail.png");
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::B))
				player2.m_subtexture = texture_atlas.findSubTexture("1-up.png");

			frame_sum -= frame_limit;
			frame_count++;
		}

		std::cout << "\r" << "sprites:" << sprites.size() << " render:" << 1.0f / frame_time.asSeconds() << " update:" << 1.0f / frame_clock.getElapsedTime().asSeconds();

		window.clear(sf::Color::Black);

		sprite_batch.clear();

		for(const moony::Sprite& sprite : sprites)
			sprite_batch.draw(sprite);

		sprite_batch.draw(player1);
		sprite_batch.draw(player2);

		sprite_batch.order();

		window.draw(sprite_batch);

		window.display();
	}

	return 0;
}

#endif