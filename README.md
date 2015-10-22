# MoonySpriteBatch
A collection of C++ code aimed at rendering lots of sprites faster with SFML


## What Is It?

MoonySpriteBatch is a combination of two projects that I have been working on to optimize drawing lots of 2D quads. Drawing many sprites with just one texture is not too difficult on even older platforms but the challenge arises when you need to incorporate layers and many different textures. By placing all your textures in one file you can overcome this and that is what I've tried to do. This is great for particle effects or bullet hell type games where you need to draw a lot of things at once.

## How To Build

Both the Texture Packer and the Example depend on SFML and Zlib. Currently I haven't made an appropriate CMake file so if you want to build it on your own setup you have to edit the CmakeLists.txt.. I'll be working on this soon.

## How To Use

The library is made up of just 2 headers (Log.h is a bonus). Those are SpriteBatch.h and TextureAtlas.h. These files know everything about loading texture atlases created by the Texture Packer and how to draw sprites.

## Texture Packer

Once built you can drop the Texture Packer tool in any directory you want but it is most useful when usd from the commandline. When you run it without any args by default it will sniff around it's current directory for any images to pack. Texture pack files are named after the folder they were created in and texture pack images begin with ta_[NUMBER][FOLDER], for instance: ta_0myimages.png.

Here are the options you can pass to the tool:

-h : Prints a help message with the list of options
-b : Tells the tool not to produce seperate atlas image files but rather compress the raw color data and pack everything into the .mtpf texture pack file
-f [FOLDER] : Designate a folder you specifically want the tool to work in
-r : Recursivley search through all directories under the starting directory. This can be used together with the -f flag
-d [COUNT]: Produce [COUNT] # of debug textures.
-v : Outputs more messages.

## Example

The moony::TextureManager class is in charge of loading your texture atlases you made with the tool. You should only need one instance of this object since it can load multiple texture atlases. You can search for the exact texture you want to assign to your sprite by searching by it's original name. 

Example:
moony::TextureManager textureman;

textureman.loadFromFile("pack.mtpf");

moony::Sprite sprite(textureman.findSubTexture("happytree.png"));


To draw a sprie you need a moony::RenderManager class. It works fairly similar to SFML's own Drawable class (mostly because it inherits from it). Again you should really only need one instance of this class.

Example:
moony::RenderManager renderman;

...

window.clear(sf::Color::Black);

renderman.clear();
renderman.draw(sprite);
renderman.batch();

window.draw(renderman);
window.display();

#Notice

The work on this is by no means final. Feel free to take ideas and inspiration or let me know where things can be improved (which I'm sure there are many). In the future there may be changes that break the previous version API and I make no commitment to supporting any versions other than the latest.