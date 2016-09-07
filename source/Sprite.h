#ifndef SPRITE_H
#define SPRITE_H

#include <SFML/Graphics.hpp>

#include "Texture.h"

namespace moony
{
	struct Sprite : public sf::Transformable
	{
		Sprite();
		Sprite(const Texture& subtexture, int layer = 0, sf::Color color = sf::Color::White);

		Texture m_subtexture;
		sf::Color m_color;
		int m_layer;
	};


	Sprite::Sprite()
	{
		m_layer = 0;
		m_color = sf::Color::White;
	}

	Sprite::Sprite(const Texture& subtexture, int layer, sf::Color color)
	{
		m_subtexture = subtexture;
		m_layer = layer;
		m_color = color;
	}
}

#endif