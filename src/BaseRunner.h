#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;
class BaseRunner : private sf::NonCopyable
{
public:
	static const sf::Time	TIME_PER_FRAME;
	static const int WINDOW_WIDTH = 1920;
	static const int WINDOW_HEIGHT = 1080;
	inline static bool isLoading = true;
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
};


