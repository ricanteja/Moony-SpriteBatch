#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H


#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <zlib.h>

#include "Log.h"

namespace moony
{
	struct SubTexture
	{
		SubTexture(const sf::Texture* texture = NULL, sf::IntRect rect = sf::IntRect()) : mTexture(texture), mTextureRect(rect) {}

		const sf::Texture* mTexture;
		sf::IntRect mTextureRect;
	};

	struct TextureAtlas
	{
		sf::Texture mTexture;
		std::unordered_map<std::string, sf::IntRect> mSubTextureTable;
	};

	class TextureManager
	{
	public:
		bool loadFromFile(const std::string& file_path)
		{
			std::ifstream input_file(file_path, std::ifstream::in | std::ifstream::binary);
			
			if(!input_file)
				return false;

			std::string input;

			while(input_file >> input)
			{
				mTextureAtlases.push_back(TextureAtlas());
				
				if(input == "D")
				{
					sf::Vector2u img_size;
					unsigned long comp_size;

					(input_file >> img_size.x >> img_size.y >> comp_size).ignore(1);

					unsigned long data_size = img_size.x * img_size.y * 4;
					char* data = new char[data_size];

					input.resize(comp_size);
					input_file.read(&input[0], comp_size);
					uncompress((Bytef*)data, &data_size, (Bytef*)&input[0], comp_size);

					mTextureAtlases.back().mTexture.create(img_size.x, img_size.y);
					mTextureAtlases.back().mTexture.update((sf::Uint8*)data);

					delete data;

					// For debugging
					//mTextureAtlases.back().mTexture.copyToImage().saveToFile("dump.png");
				}
				else
				{
					size_t name_len;
					(input_file >> name_len).ignore(1);
					input.resize(name_len);
					input_file.read(&input[0], name_len);

					mTextureAtlases.back().mTexture.loadFromFile(input);
				}

				size_t img_count;
				input_file >> img_count;

				for(size_t index = 0; index < img_count; index++)
				{
					size_t name_len;
					(input_file >> name_len).ignore(1);
					input.resize(name_len);
					input_file.read(&input[0], name_len);

					sf::IntRect texture_rect;
					input_file >> texture_rect.left >> texture_rect.top >> texture_rect.width >> texture_rect.height;

					mTextureAtlases.back().mSubTextureTable[input] = texture_rect;
				}
			}

			input_file.close();

			if(!mTextureAtlases.size())
				return false;

			return true;
		}

		const SubTexture findSubTexture(std::string name)
		{
			SubTexture subtexture;

			for(TextureAtlas& atlas : mTextureAtlases)
			{
				if(atlas.mSubTextureTable.count(name) > 0)
				{
					subtexture = SubTexture(&atlas.mTexture, atlas.mSubTextureTable[name]);
					break;
				}
			}


			return subtexture;
		}

		std::vector<std::string> getSubTextureNames()
		{
			std::vector<std::string> names;

			for(const TextureAtlas& atlas : mTextureAtlases)
			{
				for(const auto& itr : atlas.mSubTextureTable)
					names.push_back(itr.first);
			}

			return names;
		}

	private:
		std::vector<TextureAtlas> mTextureAtlases;
	};
}




#endif