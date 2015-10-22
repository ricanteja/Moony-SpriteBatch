#ifndef TEXTUREPACKER_H
#define TEXTUREPACKER_H

// The default max size is 2048 just to be on the safe side of portability
#define MAX_TEXTURE_SIZE 2048
// The padding is to avoid edge bleeding on GPUs that don't support texture edge clamping
#define TEXTURE_PADDING 1
// The max number of images to generate if debugging
#define MAX_IMAGE_COUNT 1024

#include <vector>
#include <stack>
#include <random>
#include <SFML/Graphics.hpp>

#include "Log.h"


struct SubImage
{
	sf::Image mImage;
	std::string	mName;
};

struct TextureNode
{
	sf::IntRect	mRect;
	std::string	mName;
	size_t mLargeNode;
	size_t mSmallNode;
};

struct TexturePack
{
	TexturePack(unsigned int width, unsigned int height)
	{
		if(!mTexture.create(width, height))
		{
			logError() << "Could not create Texture Atlas\n";
			abort();
		}

		mImageCount = 0;
		mCropSize = sf::Vector2u(0, 0);
		TextureNode root;
		root.mRect = sf::IntRect(0, 0, width, height);
		root.mSmallNode = 0;
		root.mLargeNode = 0;

		mNodeList.push_back(root);
	}

	bool placeSubImage(SubImage subimage)
	{
		std::stack<size_t> stack;
		// Push index 0 to start at the beginning of the list
		stack.push(0);
		// Get the size of the image to be packed and add the padding to it size
		sf::Vector2u size = subimage.mImage.getSize() + sf::Vector2u(TEXTURE_PADDING, TEXTURE_PADDING);

		while(!stack.empty())
		{
			size_t index = stack.top();

			// Check whether the current node has children
			if(!mNodeList[index].mSmallNode && !mNodeList[index].mLargeNode)
			{
				// Make sure that the image fits inside the indexed node
				if(size.x <= mNodeList[index].mRect.width && size.y <= mNodeList[index].mRect.height)
				{
					TextureNode small_node = mNodeList[index];
					TextureNode large_node = mNodeList[index];

					// Split the orignal node into two smaller rects
					large_node.mRect.top += size.y;
					large_node.mRect.height -= size.y;

					small_node.mRect.left += size.x;
					small_node.mRect.width -= size.x;
					small_node.mRect.height = size.y;

					mImageCount++;

					if(mNodeList[index].mRect.left + size.x > mCropSize.x)
						mCropSize.x = mNodeList[index].mRect.left + size.x;

					if(mNodeList[index].mRect.top + size.y > mCropSize.y)
						mCropSize.y = mNodeList[index].mRect.top + size.y;

					// Push the children nodes onto the list and store their index in the parent node
					mNodeList.push_back(small_node);
					mNodeList[index].mSmallNode = mNodeList.size() - 1;

					mNodeList.push_back(large_node);
					mNodeList[index].mLargeNode = mNodeList.size() - 1;

					// Set the indexed node with the image name and size
					mNodeList[index].mName = subimage.mName;
					mNodeList[index].mRect.width = size.x - TEXTURE_PADDING;
					mNodeList[index].mRect.height = size.y - TEXTURE_PADDING;

					// Update the texture with the image data and the position of the node
					mTexture.update(subimage.mImage, mNodeList[index].mRect.left, mNodeList[index].mRect.top);
					logDebug() << "Placing " << subimage.mName
						<< " at " << mNodeList[index].mRect.left << ", " << mNodeList[index].mRect.top << "\n";

					return true;
				}

				// If the image did not fit in the node at the current index then pop that index
				stack.pop();
			}
			else
			{
				// If the node had children, push the index of the children nodes onto the stack
				// Remove the current index from the stack first
				stack.pop();
				// Push the larger first and the the smaller because stacks are LIFO and we want to check the smaller one first
				stack.push(mNodeList[index].mLargeNode);
				stack.push(mNodeList[index].mSmallNode);
			}
		}

		return false;
	}

	sf::Image getCropImage()
	{
		sf::Texture temp;
		temp.loadFromImage(mTexture.copyToImage(), sf::IntRect(0, 0, mCropSize.x, mCropSize.y));
		return temp.copyToImage();
	}

	size_t mImageCount;
	sf::Vector2u mCropSize;
	sf::Texture mTexture;
	std::vector<TextureNode> mNodeList;
};

// Generates a list of debug images of varying sizes
std::vector<SubImage> generateDebugImages(unsigned int count)
{
	if(count > MAX_IMAGE_COUNT)
		count = MAX_IMAGE_COUNT;

	std::vector<SubImage> subimage_list;

	for(unsigned int index = 0; index < count; index++)
	{
		SubImage subimage;

		// "Randomly" generate a color 0 to 255 and a size
		sf::Color color(std::rand() % 255, std::rand() % 255, std::rand() % 255);
		sf::Vector2u size(32 + (std::rand() % (MAX_TEXTURE_SIZE / 16)), 32 + (std::rand() % (MAX_TEXTURE_SIZE / 16)));

		// Create the image, name it and push it to the list
		subimage.mImage.create(size.x, size.y, color);
		subimage.mName = "debug_image" + std::to_string(index);

		subimage_list.push_back(subimage);
	}

	return subimage_list;
}

#endif