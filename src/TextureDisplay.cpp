#include "TextureDisplay.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "IconObject.h"
TextureDisplay::TextureDisplay(): AGameObject("TextureDisplay")
{
	
}

void TextureDisplay::initialize()
{
	
}

void TextureDisplay::processInput(sf::Event event)
{
	
}

void TextureDisplay::update(sf::Time deltaTime)
{
	this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

	if (!startedStreaming)
	{
		if (this->ticks >= this->STREAMING_LOAD_DELAY)
		{
			startedStreaming = true;
			this->ticks = 0.0f;
		}
		return;
	}

	// Stream a new texture every ~75ms to visualize progressive loading
	const float LOAD_INTERVAL_MS = 75.0f;
	if (this->ticks >= LOAD_INTERVAL_MS)
	{
		this->ticks = 0.0f;
		int nextIndex = TextureManager::getInstance()->getNumLoadedStreamTextures();
		if (TextureManager::getInstance()->loadSingleStreamAsset(nextIndex))
		{
			this->spawnObject();
		}
	}
}

void TextureDisplay::spawnObject()
{
	String objectName = "Icon_" + to_string(this->iconList.size());
	IconObject* iconObj = new IconObject(objectName, this->iconList.size());
	this->iconList.push_back(iconObj);

	//set position
	int IMG_WIDTH = 68; int IMG_HEIGHT = 68;
	float x = this->columnGrid * IMG_WIDTH;
	float y = this->rowGrid * IMG_HEIGHT;
	iconObj->setPosition(x, y);

	std::cout << "Set position: " << x << " " << y << std::endl;

	this->columnGrid++;
	if(this->columnGrid == this->MAX_COLUMN)
	{
		this->columnGrid = 0;
		this->rowGrid++;
	}
	GameObjectManager::getInstance()->addObject(iconObj);
}
