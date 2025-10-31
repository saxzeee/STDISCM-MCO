#pragma once
#include "AGameObject.h"
#include <queue>
#include <memory>

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
	const float STREAMING_LOAD_DELAY = 4000.0f;
	const StreamingType streamingType = BATCH_LOAD;
	float ticks = 0.0f;
	bool startedStreaming = false;

	int columnGrid = 0; int rowGrid = 0;
	
	const int MAX_COLUMN = 28;
	const int MAX_ROW = 22;

	// thread pool + scheduling
	std::unique_ptr<ThreadPool> m_pool;
	float schedulerTicksMs = 0.0f;
	const float SCHEDULE_INTERVAL_MS = 20.0f; // short delay to build a sizeable ready queue
	int maxIconsToShow = 50;
	int promotedPerFrame = 4; // promote a small number each frame to avoid FPS drops

	void spawnObject();
};

