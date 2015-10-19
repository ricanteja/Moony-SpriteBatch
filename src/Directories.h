#ifndef TEXTUREDIRECTORY_H
#define TEXTUREDIRECTORY_H


#if WIN32
#include <Windows.h>
#else
// Haven't implemented Linux version yet
#include <dirent.h>
#endif

#include <vector>
#include <string>
#include <cstring>

#include "../include/Log.h"
#include "TexturePacker.h"


// Gets the current directory from which the program was ran
std::string getCurrentDirectory()
{
	char current_dir[2048];

#if WIN32
	GetCurrentDirectoryA(2048, current_dir);
#else
	// Linux code
#endif

	return current_dir;
}

// Recursively build the directory list beginning at [current_dir] as the root
void buildDirectoryList(std::string current_dir, std::vector<std::string>& dir_list)
{
	dir_list.push_back(current_dir);
	std::string search_dir = current_dir + "\\*.*";

#if WIN32
	WIN32_FIND_DATAA fileData;
	HANDLE fileHandle = FindFirstFileA(search_dir.c_str(), &fileData);

	if(fileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// On Windows the FindFirstFile and FindNextFile functions return empty folders "." and ".."
			// We want to check and avoid them because we know they won't have images
			if(std::strcmp(".", fileData.cFileName) != 0 && std::strcmp("..", fileData.cFileName) != 0)
			{
				// Make sure the file we got is a directory (folder)
				if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					buildDirectoryList(current_dir + "\\" + fileData.cFileName, dir_list);
			}

		} while(FindNextFileA(fileHandle, &fileData));
	}
	else
	{
		// If there was a problem getting the first file then print an error message
		char message[2048];
		FormatMessageA(0, 0, GetLastError(), 0, message, 2048, NULL);
		std::cerr << message;
	}

#else
	// Linux code
#endif
}

// Gets the list of all the folders under including the provided starting directory
std::vector<std::string> getDirectoryList(std::string directory)
{
	std::vector<std::string> dir_list;

	buildDirectoryList(directory, dir_list);

	return dir_list;
}

// Get a list of all the images inside the given directory
std::vector<SubImage> getImageList(std::string directory)
{
	std::vector<SubImage> subimage_list;
	std::string search_dir = directory + "\\*.*";

#if WIN32
	WIN32_FIND_DATAA fileData;
	// 
	HANDLE fileHandle = FindFirstFileA(search_dir.c_str(), &fileData);

	if(fileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			// Make sure the file is not a directory
			if(fileData.dwFileAttributes &~FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string file_name = fileData.cFileName;
				std::string check = file_name.substr(1 + file_name.find_last_of("."));

				// Check whether the file type is an image file
				if(check == "png" || check == "jpg" || check == "bmp" || check == "tga" ||
				   check == "hdr" || check == "gif" || check == "psd" || check == "pic")
				{
					// Make sure we didn't grab a previously made TextureAtlas
					if(file_name.substr(0, 2) != "ta")
					{
						SubImage subimage;
						subimage.mName = fileData.cFileName;

						// If the image failed to load then output an error message
						if(!subimage.mImage.loadFromFile(directory + "\\" + fileData.cFileName))
							logError() << "Failed to load image " << fileData.cFileName << "\n";
						else
							// Else put the image on the list of images
							subimage_list.push_back(subimage);
					}
				}
			}
		} while(FindNextFileA(fileHandle, &fileData));
	}
	else
	{
		// If there was a problem getting the first file on Windows, print an error message
		char message[2048];
		FormatMessageA(0, 0, GetLastError(), 0, message, 2048, NULL);
		logError() << message;
	}

#else
	// Linux code
#endif

	return subimage_list;
}

#endif