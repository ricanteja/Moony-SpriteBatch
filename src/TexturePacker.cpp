// MoonyTexturePacker
// by Ricardo Antonio Tejada
//
// This tool packs multiple texture files into one larger file.
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
#include <sstream>
#include <random>
#include <zlib.h>
#include <SFML/Graphics.hpp>

#include "Log.h"

#include "TexturePacker.h"
#include "Directories.h"


void printHelp()
{
	std::cout << "                Moony-TexturePacker\n"
		<< "        by Ricardo Antonio Tejada 2015.\n\n"
		<< "To use the program invoke it through the commandline\n"
		<< "The program takes flags to modify behavior, these are:\n"
		<< "help flag       [-h]\n\tPrints this help message\n"
		//<< "size flag       [-s WIDTH HEIGHT]\n\tSet the output size (power of two size recommended)\n"
		<< "binary flag     [-b]\n\tOutput single binary file\n"
		<< "target folder   [-f FOLDER]\n\tSpecify the location of the images to be packed\n"
		<< "recursive flag  [-r]\n\tRecursively search sub-directories\n"
		<< "debug flag      [-d IMAGE_COUNT]\n\tTest the program and generate test images\n"
		<< "verbose flag    [-v]\n\tOutputs more messages and logs them to file\n";

	std::cout << "\n\nThank you for packing your textures with Moony-TexturePacker\nPress any key to exit.";
}

int main(int argc, char* argv[])
{
	sf::Vector2u texture_size(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE);
	bool flag_verbose = false;
	bool flag_binary = false;
	std::string file_folder = getCurrentDirectory();
	bool flag_recurse = false;
	bool flag_debug = false;
	unsigned int debug_count = 500;

	if(argc > 1)
	{
		for(unsigned int index = 1; index < argc; index++)
		{
			if(std::strcmp(argv[index], "-h") == 0)
			{
				printHelp();
				return 0;
			}
			/*else if(std::strcmp(argv[index], "-s") == 0) // So broken..
			{
				flag_setsize = true;

				if(index + 2 < argc)
				{
					texture_size.x = std::atoi(argv[++index]);
					texture_size.y = std::atoi(argv[++index]);
					texture_size.x = texture_size.x > 0 && texture_size.x < MAX_TEXTURE_SIZE ? texture_size.x : MAX_TEXTURE_SIZE;
					texture_size.y = texture_size.y > 0 && texture_size.y < MAX_TEXTURE_SIZE ? texture_size.y : MAX_TEXTURE_SIZE;
				}
				else
					logError() << "Not enough arguments, using default\n";
			}*/
			else if(std::strcmp(argv[index], "-b") == 0)
				flag_binary = true;
			else if(std::strcmp(argv[index], "-f") == 0)
				file_folder = argv[index + 1];
			else if(std::strcmp(argv[index], "-r") == 0)
				flag_recurse = !flag_debug;
			else if(std::strcmp(argv[index], "-d") == 0)
			{
				flag_debug = true;
				flag_recurse = false;

				if(index + 1 < argc)
				{
					unsigned int count = std::atoi(argv[++index]);
					debug_count = count > 0 && count < MAX_IMAGE_COUNT ? count : MAX_IMAGE_COUNT;
				}
				else
					logError() << "Not enough arguments, using default\n";
			}
			else if(std::strcmp(argv[index], "-v") == 0)
				flag_verbose = true;
		}
	}

	std::vector<std::string> dir_list;

	// Get the folders that have the images we want to pack
	if(flag_recurse)
		dir_list = getDirectoryList(file_folder);
	else
		dir_list.push_back(file_folder);

	for(size_t dir_index = 0; dir_index < dir_list.size(); dir_index++)
	{
		std::string dir_path = dir_list[dir_index];
		std::vector<SubImage> subimage_list;
		
		// Get the list of images in the current directory
		if(flag_debug)
			subimage_list = generateDebugImages(debug_count);
		else
			subimage_list = getImageList(dir_path);

		if(subimage_list.size())
		{
			// Sort the images from large to small based on height
			std::sort(subimage_list.begin(), subimage_list.end(), [&](SubImage a, SubImage b)
			{
				sf::Vector2u size_a = a.mImage.getSize();
				sf::Vector2u size_b = b.mImage.getSize();

				return size_a.y > size_b.y;
			});

			// Now we are ready to pack the images into the atlas
			std::vector<TexturePack> pack_list;
			pack_list.push_back(TexturePack(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE));

			for(SubImage subimage : subimage_list)
			{
				for(size_t atlas_index = 0; atlas_index < pack_list.size(); atlas_index++)
				{
					// If if the image is smaller than the max texture size
					if(subimage.mImage.getSize().x + TEXTURE_PADDING < MAX_TEXTURE_SIZE &&
					   subimage.mImage.getSize().y + TEXTURE_PADDING < MAX_TEXTURE_SIZE)
					{
						// If the image could not fit in the indexed atlas and there are no more atlases
						if(!pack_list[atlas_index].placeSubImage(subimage) && atlas_index == pack_list.size() - 1)
							pack_list.push_back(TexturePack(MAX_TEXTURE_SIZE, MAX_TEXTURE_SIZE));
					}
				}
			}

			// Write the file
			std::string folder_name = dir_path.substr(1 + dir_path.find_last_of("/\\"));
			std::string output_name = dir_path + "/\\" + folder_name + ".mtpf";

			std::ofstream output_file(output_name, std::ofstream::out | std::ofstream::binary);

			if(!output_file)
			{
				std::cerr << "Failed to open the file " << output_name << "\n";
				return -1;
			}

			for(size_t index = 0; index < pack_list.size(); index++)
			{
				TexturePack& pack = pack_list[index];

				sf::Image image = pack.getCropImage();
				texture_size = pack.mCropSize;

				if(flag_binary)
				{
					unsigned long data_size = texture_size.x * texture_size.y * 4;
					unsigned long comp_size = compressBound(data_size);
					char* comp_dest = new char[comp_size];

					compress((Bytef*)comp_dest, &comp_size, image.getPixelsPtr(), data_size);

					output_file << "D " << texture_size.x << " " << texture_size.y << " " << comp_size << " ";
					output_file.write(comp_dest, comp_size);
				}
				else
				{
					output_name = dir_path + "/\\" + std::to_string(index) + folder_name + ".png"; /* FIX ME */
					image.saveToFile(output_name);

					output_file << "F " << output_name.size() << " ";
					output_file.write(&output_name[0], output_name.size());
				}
				
				output_file << pack.mImageCount << " ";

				for(TextureNode node : pack.mNodeList)
				{
					if(node.mSmallNode && node.mLargeNode)
					{
						output_file << node.mName.size() << " ";
						output_file.write(&node.mName[0], node.mName.size()) << node.mRect.left
							<< " " << node.mRect.top << " " << node.mRect.width << " " << node.mRect.height << " ";
					}
				}
			}

			output_file.close();
		}
	}

	// If verbose is on then write out the log.txt file
	if(flag_verbose)
	{
		std::cout << logGetStream().str();
		logSaveToFile("log.txt", std::fstream::out);
	}

	std::cout << "\nThank you for packing your textures with MoonyTexturePacker!";

	return 0;
}
