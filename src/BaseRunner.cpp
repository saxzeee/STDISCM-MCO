#include "BaseRunner.h"
#include  "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "TextureDisplay.h"
#include "FPSCounter.h"

/// <summary>
/// This demonstrates a running parallax background where after X seconds, a batch of assets will be streamed and loaded.
/// </summary>
const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    //window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "HO: Entity Component", sf::Style::Default) {
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "PS2: Interactive Loading Screen", sf::Style::Default) {
    // Cap the framerate to 60 FPS
    this->window.setFramerateLimit(60);

    //load initial textures
    TextureManager::getInstance()->loadFromAssetList();
    //loading icon
    loadingTexture.loadFromFile("Media/loading.png");
    loadingSprite.setTexture(loadingTexture);
    loadingSprite.setOrigin(loadingTexture.getSize().x / 2, loadingTexture.getSize().y / 2);
    loadingSprite.setScale(0.3f, 0.3f);
    loadingSprite.setPosition(WINDOW_WIDTH / 10, WINDOW_HEIGHT / 1.25);
    
    //load objects
    BGObject* bgObject = new BGObject("BGObject");
    GameObjectManager::getInstance()->addObject(bgObject);

    TextureDisplay* display = new TextureDisplay();
    GameObjectManager::getInstance()->addObject(display);

    FPSCounter* fpsCounter = new FPSCounter();
    GameObjectManager::getInstance()->addObject(fpsCounter);
}

void BaseRunner::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (this->window.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > TIME_PER_FRAME)
        {
            timeSinceLastUpdate -= TIME_PER_FRAME;
            processEvents();
            update(TIME_PER_FRAME);
        }

        render();
    }
}

void BaseRunner::processEvents()
{
    sf::Event event;
    while (this->window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Resized:
        {
            sf::View view(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
            this->window.setView(view);
            GameObjectManager::getInstance()->processInput(event);
            break;
        }
        case sf::Event::Closed:
            this->window.close();
            break;
        default:
            GameObjectManager::getInstance()->processInput(event);
            break;
        }
    }
}

void BaseRunner::update(sf::Time elapsedTime) {
    GameObjectManager::getInstance()->update(elapsedTime);
}

void BaseRunner::render() {
    this->window.clear();
    GameObjectManager::getInstance()->draw(&this->window);
    if (isLoading) {
        loadingSprite.rotate(3);
        this->window.draw(loadingSprite);
    }
    this->window.display();
}