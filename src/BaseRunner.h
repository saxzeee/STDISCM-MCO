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
	inline static bool isLoading = true;
	inline static std::string musicFilePath = "";

	enum class BGPhase { Splash, HoldAfterLoad, FadingOut, Final };
	inline static BGPhase bgPhase = BGPhase::Splash;

	inline static bool isFading = false;
	inline static float fadeAlpha = 255.0f; // 255 -> 0
	float fadeElapsedMs = 0.0f;
	const float FADE_DURATION_MS = 300.0f;

	float postLoadHoldMs = 500.0f; // adjust for longer delay
	float holdElapsedMs = 0.0f;

	inline static std::size_t totalAssets = 0;
	inline static std::size_t assetsLoaded = 0;

	sf::Time loadTime = sf::Time::Zero;
	sf::Texture loadingTexture;
	sf::Sprite loadingSprite;

	// Progress bar overlay geometry and fill
	sf::RectangleShape progressFill;
	float barInnerLeftOffset = 0.0f;
	float barInnerTopOffset = 0.0f;
	float barInnerWidth = 0.0f;
	float barInnerHeight = 0.0f;

	BaseRunner();
	void run();

private:
	
	sf::RenderWindow		window;
	void render();
	void processEvents();
	void update(sf::Time elapsedTime);
	void configureProgressBarGeometry();
	void updateProgressBar();
	sf::Color progressColorFor(float t) const;

	sf::Music loadingMusic;  
	sf::Music backgroundMusic;
	bool musicStarted = false;
	bool loadingMusicStarted = false;
	bool isCrossfading = false;
	float crossfadeTime = 0.0f;
	const float CROSSFADE_DURATION = 1.5f;
};


