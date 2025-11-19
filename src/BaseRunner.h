#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

using namespace std;
class BaseRunner : private sf::NonCopyable
{
public:
	static const sf::Time	TIME_PER_FRAME;
	static const int WINDOW_WIDTH = 1920;
	static const int WINDOW_HEIGHT = 1080;
	inline static bool isLoading = true; // true until all streaming assets promoted
	inline static std::string musicFilePath = "";

	// Explicit background phase to avoid repeated fading
	enum class BGPhase { Splash, FadingOut, Final }; // Splash_9_0 -> FadingOut -> Splash_1_0
	inline static BGPhase bgPhase = BGPhase::Splash;

	inline static bool isFading = false;  // convenience flag used only during FadingOut
	inline static float fadeAlpha = 255.0f; // 255 -> 0
	float fadeElapsedMs = 0.0f;
	const float FADE_DURATION_MS = 300.0f;

	sf::Time loadTime = sf::Time::Zero;
	sf::Texture loadingTexture;
	sf::Sprite loadingSprite;
	BaseRunner();
	void run();

private:
	
	sf::RenderWindow		window;
	void render();
	void processEvents();
	void update(sf::Time elapsedTime);
	sf::Music loadingMusic;  
	sf::Music backgroundMusic;
	bool musicStarted = false;
	bool loadingMusicStarted = false;
	bool isCrossfading = false;
	float crossfadeTime = 0.0f;
	const float CROSSFADE_DURATION = 1.5f;
};


