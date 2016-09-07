#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

// Number of frames a batch can be empty for before it is destroyed

#include <vector>
#include <array>
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "Texture.h"
#include "Sprite.h"


namespace moony
{
	class SpriteBatch : public sf::Drawable
	{
	public:
		void clear();
		void draw(const Sprite& sprite);
		void order();

	private:
		struct Batch
		{
			void clear()
			{
				if(m_count > 0)
				{
					//m_batches[i].m_count = m_batches[i].m_reserved;
					m_count = 0;
					m_inactive = 0;
				}
				else
					m_inactive++;
			}

			void draw(const Sprite& sprite)
			{
				size_t vertex_count = m_count * 4;

				if(vertex_count + 4 > m_vertices.size())
					m_vertices.resize(m_vertices.size() + 4);

				sf::Vector2f position(sprite.m_subtexture.m_rect.left, sprite.m_subtexture.m_rect.top);
				sf::Vector2f size(sprite.m_subtexture.m_rect.width, sprite.m_subtexture.m_rect.height);

				sf::Transform transform = sprite.getTransform();

				m_vertices[vertex_count].position = transform.transformPoint(0.0f, 0.0f);
				m_vertices[vertex_count].texCoords = position;
				m_vertices[vertex_count].color = sprite.m_color;

				m_vertices[vertex_count + 1].position = transform.transformPoint(0.0f, size.y);
				m_vertices[vertex_count + 1].texCoords = sf::Vector2f(position.x, size.y + position.y);
				m_vertices[vertex_count + 1].color = sprite.m_color;

				m_vertices[vertex_count + 2].position = transform.transformPoint(size);
				m_vertices[vertex_count + 2].texCoords = size + position;
				m_vertices[vertex_count + 2].color = sprite.m_color;

				m_vertices[vertex_count + 3].position = transform.transformPoint(size.x, 0.0f);
				m_vertices[vertex_count + 3].texCoords = sf::Vector2f(size.x + position.x, position.y);
				m_vertices[vertex_count + 3].color = sprite.m_color;

				m_count++;
			}

			const sf::Texture* m_texture;
			int m_layer;
			unsigned int m_inactive;
			unsigned int m_count;
			//unsigned int m_reserved;
			std::vector<sf::Vertex> m_vertices;
		};

		Batch* findBatch(const Sprite& sprite);
		void draw(sf::RenderTarget& target, sf::RenderStates state) const;

		std::vector<Batch*> m_batches;
	};



	void SpriteBatch::clear()
	{
		for(unsigned int i = 0; i < m_batches.size(); i++)
		{
			m_batches[i]->clear();

			if(m_batches[i]->m_inactive > 256)
			{
				std::swap(m_batches[i], m_batches.back());
				delete m_batches.back();
				m_batches.pop_back();
				i--;
			}
		}
	}



	SpriteBatch::Batch* SpriteBatch::findBatch(const Sprite& sprite)
	{
		for(Batch* batch : m_batches)
		{
			if(batch->m_texture == sprite.m_subtexture.m_texture && batch->m_layer == sprite.m_layer)
				return batch;
		}

		Batch* batch = new Batch();
		batch->m_texture = sprite.m_subtexture.m_texture;
		batch->m_layer = sprite.m_layer;
		batch->m_inactive = 0;
		batch->m_count = 0;

		m_batches.push_back(batch);
		return m_batches.back();
	}



	void SpriteBatch::draw(const Sprite& sprite)
	{
		if(sprite.m_subtexture.m_texture == NULL)
			return;

		findBatch(sprite)->draw(sprite);
	}



	void SpriteBatch::order()
	{
		// Sort the batches
		std::sort(m_batches.begin(), m_batches.end(), [&](const Batch* a_batch, const Batch* b_batch)
		{
			if(a_batch->m_layer == b_batch->m_layer)
				return a_batch->m_texture < a_batch->m_texture;

			return a_batch->m_layer < a_batch->m_layer;
		});
	}



	void SpriteBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for(const Batch* batch : m_batches)
		{
			states.texture = batch->m_texture;
			target.draw(batch->m_vertices.data(), batch->m_count * 4, sf::PrimitiveType::Quads, states);
		}
	}
}



#endif