#include "TextureDisplay.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"
#include "GameObjectManager.h"
#include "IconObject.h"
#include "ThreadPool.h"
#include <filesystem>

TextureDisplay::TextureDisplay(): AGameObject("TextureDisplay")
{
	
}

void TextureDisplay::initialize()
{
	// create a small pool; 4 threads is plenty for IO/decode
	m_pool = std::make_unique<ThreadPool>(4);

	// Pre-enqueue CPU decode jobs so we build a sizeable ready queue
	std::vector<std::string> files;
	TextureManager::getInstance()->enumerateStreamingFiles(files);
	for (const auto& path : files)
	{
		m_pool->enqueue([path]() {
			// Decode image on worker thread
			sf::Image img;
			if (!img.loadFromFile(path)) return;
			// Extract asset name (filename without extension)
			auto fname = std::filesystem::path(path).filename().string();
			auto pos = fname.find_last_of('.');
			TextureManager::String assetName = (pos == std::string::npos) ? fname : fname.substr(0, pos);

			TextureManager::DecodedImage di{assetName, std::make_shared<sf::Image>(std::move(img))};
			TextureManager::getInstance()->pushReadyImage(std::move(di));
		});
	}
}

void TextureDisplay::processInput(sf::Event event)
{
	
}

void TextureDisplay::update(sf::Time deltaTime)
{
	this->ticks += BaseRunner::TIME_PER_FRAME.asMilliseconds();

	//<code here for spawning icon object periodically>
	if (!startedStreaming)
	{
		if (this->ticks >= this->STREAMING_LOAD_DELAY)
		{
			startedStreaming = true;
			this->ticks = 0.0f;
		}
		return;
	}

	// Promote a bounded number of decoded images to GPU textures per frame
	int promoted = 0;
	while (promoted < promotedPerFrame && (int)iconList.size() < maxIconsToShow)
	{
		TextureManager::DecodedImage di;
		if (!TextureManager::getInstance()->popReadyImage(di)) break;
		if (TextureManager::getInstance()->registerReadyImageToTexture(di))
		{
			// after registering, we can try to spawn an icon for the new texture
			this->spawnObject();
		}
		promoted++;
	}

	// Ensure at least 50 icons appear (as textures become available)
	while ((int)iconList.size() < std::min(maxIconsToShow, TextureManager::getInstance()->getNumLoadedStreamTextures()))
	{
		this->spawnObject();
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
