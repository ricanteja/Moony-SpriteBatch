#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

// Number of frames a batch can be empty for before it is destroyed

#include <vector>
#include <array>
#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

#include "TextureAtlas.h"


namespace moony
{
	struct Sprite : public sf::Transformable
	{
		Sprite()
		{
			mTexture = NULL;
			mTextureRect = sf::IntRect();
			mLayer = 0;
			mColor = sf::Color::White;
		}

		Sprite(const sf::Texture& texture, sf::IntRect rect, int layer = 0, sf::Color color = sf::Color::White)
		{
			mTexture = &texture;
			mTextureRect = rect;
			mLayer = layer;
			mColor = color;
		}

		Sprite(const SubTexture& subtexture, int layer = 0, sf::Color color = sf::Color::White)
		{
			setSubTexture(subtexture);
			mLayer = layer;
			mColor = color;
		}
	
		void setSubTexture(const SubTexture& subtexture)
		{
			if(subtexture.mTexture != mTexture || subtexture.mTextureRect != mTextureRect)
			{
				mTexture = subtexture.mTexture;
				mTextureRect = subtexture.mTextureRect;
			}
		}

		const sf::Texture* mTexture;
		sf::IntRect mTextureRect;
		sf::Color mColor;
		int mLayer;
	};

	struct SpriteBatch
	{
		SpriteBatch(const Sprite& sprite)
		{
			mTexture = sprite.mTexture;
			mLayer = sprite.mLayer;
			mInactiveFrames = 0;
			mSpriteCount = 0;
		}

		const sf::Texture* mTexture;
		int mLayer;
		size_t mInactiveFrames;
		size_t mSpriteCount;
		std::vector<sf::Vertex> mVertices;
	};

	class RenderManager : public sf::Drawable
	{
	public:
		void clear()
		{
			for(size_t index = 0; index < mBatchList.size(); index++)
			{
				if(mBatchList[index].mSpriteCount > 0)
				{
					mBatchList[index].mSpriteCount = 0;
					mBatchList[index].mInactiveFrames = 0;
				}
				/*else 
				{
					if(mBatchList[index].mInactiveFrames > 256)
					{
						mBatchList[index] = mBatchList.back();
						mBatchList.pop_back();
						index--;
					}
					else
						mBatchList[index].mInactiveFrames++;
				}*/
			}
		}

		void draw(const Sprite& sprite)
		{
			if(sprite.mTexture == NULL)
				return;

			SpriteBatch* batch_ptr = NULL;
			SpriteBatch* found_ptr = NULL;

			for(SpriteBatch& batch : mBatchList)
			{
				if(batch.mTexture == sprite.mTexture && batch.mLayer == sprite.mLayer)
				{
					batch_ptr = &batch;
					break;
				}
				else if(batch.mInactiveFrames > 0)
				{
					if(found_ptr == NULL || found_ptr->mInactiveFrames < batch.mInactiveFrames)
						found_ptr = &batch;
				}
			}

			if(batch_ptr == NULL)
				mBatchList.push_back(SpriteBatch(sprite));

			batch_ptr = &mBatchList.back();

			size_t vertex_count = batch_ptr->mSpriteCount * 4;

			if(vertex_count + 4 > batch_ptr->mVertices.size())
				batch_ptr->mVertices.resize(batch_ptr->mVertices.size() + 4);
			
			sf::Vector2f position(sprite.mTextureRect.left, sprite.mTextureRect.top);
			sf::Vector2f size(sprite.mTextureRect.width, sprite.mTextureRect.height);

			sf::Transform transform = sprite.getTransform();

			batch_ptr->mVertices[vertex_count].position = transform.transformPoint(0.0f, 0.0f);
			batch_ptr->mVertices[vertex_count].texCoords = position;
			batch_ptr->mVertices[vertex_count].color = sf::Color::White;

			batch_ptr->mVertices[vertex_count + 1].position = transform.transformPoint(0.0f, size.y);
			batch_ptr->mVertices[vertex_count + 1].texCoords = sf::Vector2f(position.x, size.y + position.y);
			batch_ptr->mVertices[vertex_count + 1].color = sf::Color::White;

			batch_ptr->mVertices[vertex_count + 2].position = transform.transformPoint(size);
			batch_ptr->mVertices[vertex_count + 2].texCoords = size + position;
			batch_ptr->mVertices[vertex_count + 2].color = sf::Color::White;

			batch_ptr->mVertices[vertex_count + 3].position = transform.transformPoint(size.x, 0.0f);
			batch_ptr->mVertices[vertex_count + 3].texCoords = sf::Vector2f(size.x + position.x, position.y);
			batch_ptr->mVertices[vertex_count + 3].color = sf::Color::White;

			batch_ptr->mSpriteCount++;
		}

		void batch()
		{
			std::sort(mBatchList.begin(), mBatchList.end(), [&](const SpriteBatch& a, const SpriteBatch& b)
			{
				return a.mLayer < b.mLayer;
			});
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const SpriteBatch& batch : mBatchList)
			{
				states.texture = batch.mTexture;
				target.draw(batch.mVertices.data(), batch.mSpriteCount * 4, sf::PrimitiveType::Quads, states);
			}
		}

	private:
		std::vector<SpriteBatch> mBatchList;
	};
}



#endif