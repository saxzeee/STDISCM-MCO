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
	m_pool = std::make_unique<ThreadPool>(4);
	TextureManager::getInstance()->enumerateStreamingFiles(m_streamFiles);

	assetCount = m_streamFiles.size(); // get the amount of files to laod
	BaseRunner::totalAssets = assetCount; // register total for progress bar
	m_nextFileIndex = 0;

	std::size_t prewarm = std::min<std::size_t>(m_streamFiles.size(), m_batchPerTick * 2);
	for (std::size_t i = 0; i < prewarm; ++i)
	{
		const auto path = m_streamFiles[i];
		m_pool->enqueue([path]() {
			// Handle mp3 files that will be loaded
			if (path.find(".mp3") != std::string::npos) {
				auto fname = std::filesystem::path(path).filename().string();
				auto pos = fname.find_last_of('.');
				TextureManager::String assetName = (pos == std::string::npos) ? fname : fname.substr(0, pos);
				TextureManager::getInstance()->markAudioAsReady(assetName, path);
				return;
			}
			sf::Image img;
			if (!img.loadFromFile(path)) return;
			auto fname = std::filesystem::path(path).filename().string();
			auto pos = fname.find_last_of('.');
			TextureManager::String assetName = (pos == std::string::npos) ? fname : fname.substr(0, pos);

			TextureManager::DecodedImage di{assetName, std::make_shared<sf::Image>(std::move(img))};
			TextureManager::getInstance()->pushReadyImage(std::move(di));
		});
	}
	m_nextFileIndex = prewarm;
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

	// Fixed-time-step scheduling: enqueue decode tasks in small batches every SCHEDULE_INTERVAL_MS
	schedulerTicksMs += BaseRunner::TIME_PER_FRAME.asMilliseconds();
	if (schedulerTicksMs >= SCHEDULE_INTERVAL_MS)
	{
		std::size_t scheduled = 0;
		while (scheduled < m_batchPerTick && m_nextFileIndex < m_streamFiles.size())
		{
			const auto path = m_streamFiles[m_nextFileIndex++];
			m_pool->enqueue([path]() {
				// check for audio file
				if (path.find(".mp3") != std::string::npos) {
					auto fname = std::filesystem::path(path).filename().string();
					auto pos = fname.find_last_of('.');
					TextureManager::String assetName = (pos == std::string::npos) ? fname : fname.substr(0, pos);
					TextureManager::getInstance()->markAudioAsReady(assetName, path);
					return;
				}
				sf::Image img;
				if (!img.loadFromFile(path)) return;
				auto fname = std::filesystem::path(path).filename().string();
				auto pos = fname.find_last_of('.');
				TextureManager::String assetName = (pos == std::string::npos) ? fname : fname.substr(0, pos);
				TextureManager::DecodedImage di{assetName, std::make_shared<sf::Image>(std::move(img))};
				TextureManager::getInstance()->pushReadyImage(std::move(di));
			});
			scheduled++;
		}
		schedulerTicksMs = 0.0f;
	}

	// Promote a bounded number of decoded images to GPU textures per frame
	int promoted = 0;
	while (promoted < promotedPerFrame)
	{
		// pop image
		TextureManager::DecodedImage di;
		if (TextureManager::getInstance()->popReadyImage(di)) {
			if (TextureManager::getInstance()->registerReadyImageToTexture(di))
			{
				assetsReady++;
				imagesReady++;
				BaseRunner::assetsLoaded = assetsReady; // update progress bar
				std::cout << "[TextureDisplay] Promoted texture: " << di.assetName << " "
					<< "assets ready: " << assetsReady << "/" << assetCount << std::endl;
			}
			promoted++;
			continue;
		}

		// pop audio
		TextureManager::AudioAsset audio;
		if (TextureManager::getInstance()->popReadyAudio(audio)) {
			assetsReady++;
			BaseRunner::assetsLoaded = assetsReady; // update progress bar
			std::cout << "[TextureDisplay] Audio ready: " << audio.assetName << " "
				<< "assets ready: " << assetsReady << "/" << assetCount << std::endl;

			// save the bg music path read from streaming folder
			BaseRunner::musicFilePath = audio.filePath;
			promoted++;
			continue;
		}

		break;
	}
	if (BaseRunner::isLoading&& assetsReady >= assetCount) {
		BaseRunner::isLoading = false; // mark loading end
	}
	if (!BaseRunner::isLoading && iconList.size() < imagesReady) {
		spawnTimerMs += BaseRunner::TIME_PER_FRAME.asMilliseconds();

		if (spawnTimerMs >= spawnDelayMs) {
			this->spawnObject();
			spawnTimerMs = 0.0f;
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
