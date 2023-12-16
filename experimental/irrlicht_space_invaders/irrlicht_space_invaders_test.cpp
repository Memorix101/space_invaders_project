// irrlicht_space_invaders.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdlib>
#include <thread>
#include <time.h>
#include <stdio.h>
#include <string>
#include <Irrlicht/irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace gui;
using namespace core;
using namespace io;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif


ITexture* sprite;
u32 currentIndex;

int main(void)
{
	IrrlichtDevice* device = createDevice(EDT_OPENGL, dimension2d<u32>(640, 480));

	if(device)
	{
		IVideoDriver* driver = device->getVideoDriver();

		sprite = driver->getTexture("./rd/invader32x32x4.png");
		currentIndex = 3;

		u32 timeStamp = device->getTimer()->getTime();

		while(device->run())
		{
			driver->beginScene(true, true, SColor(0,0,0,255));
			
				if(device->getTimer()->getTime() - timeStamp > 250)
				{
					timeStamp = device->getTimer()->getTime();
					currentIndex++;

					if(currentIndex == 4)
					{
						currentIndex = 0;
					}
				}

				driver->draw2DImage(sprite, position2d<int>(0,0), rect<s32>(0, 0, 32, 32), 0, SColor(255,255,255,255), true);

			driver->endScene();
		}

		device->drop();
	}	

	return 0;
}