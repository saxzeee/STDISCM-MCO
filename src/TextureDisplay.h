#pragma once
#include "AGameObject.h"
#include <queue>
#include <memory>
#include <vector>
#include <string>

class IconObject;
class ThreadPool;
/// <summary>
/// Class that deals with displaying of streamed textures
/// </summary>
class TextureDisplay: public AGameObject
{
public:
	TextureDisplay();
	void initialize();
	void processInput(sf::Event event);
	void update(sf::Time deltaTime);

private:
	typedef std::vector<IconObject*> IconList;
	IconList iconList;

	enum StreamingType { BATCH_LOAD = 0, SINGLE_STREAM = 1 };
	const float STREAMING_LOAD_DELAY = 3000.0f;
	const StreamingType streamingType = BATCH_LOAD;
	float ticks = 0.0f;
	bool startedStreaming = false;

	int columnGrid = 0; int rowGrid = 0;
	
	const int MAX_COLUMN = 25;
	const int MAX_ROW = 22;

	// thread pool + fixed-interval scheduling
	std::unique_ptr<ThreadPool> m_pool;
	float schedulerTicksMs = 0.0f;
	const float SCHEDULE_INTERVAL_MS = 250.0f; // change for batch loading speed
	std::vector<std::string> m_streamFiles;
	std::size_t m_nextFileIndex = 0;
	std::size_t m_batchPerTick = 10;           // number of images to load per tick
	int promotedPerFrame = 2; // promote a small number each frame to avoid FPS drops
	std::size_t assetCount = 0; // to load (loading screen)
	std::size_t assetsReady = 0; // after loading in the bg
	float spawnDelayMs = 50.0f;  // ms delay between each ic on spawn
	float spawnTimerMs = 0.0f;
	void spawnObject();
};

