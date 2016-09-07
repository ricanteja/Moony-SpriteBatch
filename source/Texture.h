#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>


namespace moony
{
	// The Texture class is similar to the SFML sf::Texture class but was designed to work specifically with the TextureAtlas class.
	struct Texture
	{
		Texture(const sf::Texture* texture = NULL, sf::IntRect rect = sf::IntRect());

		const sf::Texture* m_texture;
		sf::IntRect m_rect;
	};

	// Texture class constructor
	Texture::Texture(const sf::Texture* texture, sf::IntRect rect)
	{
		m_texture = texture;
		m_rect = rect;
	}
}


#endif