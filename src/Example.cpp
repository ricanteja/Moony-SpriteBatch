// MoonySpriteBatch
// by Ricardo Antonio Tejada
//
// This program is an example showing how one might use these tools I've written in an actual dev scenario.
// The demo will load a ,mtpf (MoonyTexturePackFile) file and draw all of the textures using a sprite batch.
// 
// Note you will need a C++11 compiler
// Flags:
//   -h                  (Prints the help screen and then quits)
//   -s WIDTH HEIGHT     (Set the output texture size; I recommend a power of two size for compatibility)
//   -b                  (Tells the program to output a single binary file containing the image and nodes)
//   -r                  (Tells the program to recursively search all sub-directories of the root directory)
//   -d IMAGE_COUNT      (Test the program with generated images; IMAGE_COUNT is the number of images to generate)
//   -v                  (Make the program verbose and create a log file)

#include <iostream>
#include <iterator>
#include <SFML/Graphics.hpp>

#include "Log.h"
#include "TextureAtlas.h"
#include "SpriteBatch.h"

int testMoony()
{
    std::random_device random;
    std::srand(random());

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Sprite Batching and Texture Atlases", sf::Style::Close);
	//window.setVerticalSyncEnabled(true);

	moony::TextureManager textureman;

	if(!textureman.loadFromFile("A/A.mtpf"))
		return -1;

	if(!textureman.loadFromFile("B/B.mtpf"))
		return -1;

    //if(!textureman.loadFromFile("A/C/C.mtpf"))
    //    return -1;

	moony::SubTexture subtexture = textureman.findSubTexture("g.png");
	moony::SubTexture thiswillfail = textureman.findSubTexture("doesnotexist.png");

	moony::RenderManager renderman;
	moony::Sprite player1(subtexture,2);
	moony::Sprite player2(textureman.findSubTexture("r.png"));

	std::vector<moony::Sprite> sprites;

	std::vector<std::string> names = textureman.getSubTextureNames();

	for(size_t index = 0; index < 10; index++)
	{
		size_t name_index = index % names.size();
		subtexture = textureman.findSubTexture(names[name_index]);

		sprites.push_back(moony::Sprite(subtexture, random() % 5));
		sprites.back().setPosition(std::rand() % 800, std::rand() % 600);
		sprites.back().setOrigin(sprites.back().mTextureRect.width / 2, sprites.back().mTextureRect.height / 2);
	}

    int loop_count = 0;
    int loop_limit = 30;
    float frame_total = 0.0f;
    float frame_time = 0.0f;
    float logic_time = 0.0f;
    float delta_time = 0.0f;
    float frame_limit = 1.0f / 60.0f;
    sf::Clock frame_clock;
    sf::Clock timer_clock;

    bool fullscreen = false;

	while(window.isOpen())
	{
		frame_time = frame_clock.restart().asSeconds();

		sf::Event event;

		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed: window.close(); break;
				default: break;
			}
		}

		loop_count = 0;
		frame_total += frame_time;

		while(frame_total > frame_limit && loop_count < loop_limit)
		{
			// Test movement
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				player1.move(0, -5);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				player1.move(0, 5);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				player1.move(-5, 0);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				player1.move(5, 0);
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
			{
				window.create(sf::VideoMode(800, 600), "SFML Sprite Batching and Texture Atlases", fullscreen ? sf::Style::Fullscreen : sf::Style::Close);
				fullscreen = !fullscreen;
			}

			float num = std::cos(timer_clock.getElapsedTime().asSeconds());

			for(moony::Sprite& sprite : sprites)
			{
				sprite.rotate(num * 10);
				sprite.setScale(num, num);
				//sprite.move(num, num);
			}

			frame_total -= frame_limit;
			++loop_count;
		}

		// Test texture changing
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
			player1.mTextureRect = textureman.findSubTexture("gottagofast.png").mTextureRect;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::H))
			player1.mTextureRect = textureman.findSubTexture("debug_image10").mTextureRect;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::N))
			player1.mTextureRect = textureman.findSubTexture("debug_image15").mTextureRect;

		logic_time = frame_clock.getElapsedTime().asSeconds();
		delta_time = frame_total / frame_limit;

		std::cout << "\n" << "render:" << 1.0f / frame_time << " update:" << 1.0f / logic_time << "delta:" << delta_time;

		window.clear(sf::Color::Black);
		renderman.clear();

		for(const moony::Sprite& sprite : sprites)
			renderman.draw(sprite);

		renderman.draw(player1);
        renderman.draw(player2);

		renderman.batch();

		window.draw(renderman);

		window.display();
	}

	return 0;
}

int main()
{
	testMoony();

	return 0;
}