#include "BaseRunner.h"
#include  "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "TextureDisplay.h"
#include "FPSCounter.h"
#include <iostream>

/// <summary>
/// This demonstrates a running parallax background where after X seconds, a batch of assets will be streamed and loaded.
/// </summary>
const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "PS2: Interactive Loading Screen", sf::Style::Default) {
    this->window.setFramerateLimit(60);

    //load initial textures (includes Splash_9_0 via Media/assets.txt)
    TextureManager::getInstance()->loadFromAssetList();

    if (loadingMusic.openFromFile("Media/Terraria_Title_OST.mp3")) {
        loadingMusic.setLoop(true);
        loadingMusic.setVolume(15);
        loadingMusic.play();
        loadingMusicStarted = true;
    }

    loadingTexture.loadFromFile("Media/loading.png");
    loadingSprite.setTexture(loadingTexture);
    loadingSprite.setOrigin(loadingTexture.getSize().x / 2.f, loadingTexture.getSize().y / 2.f);
    loadingSprite.setScale(0.3f, 0.3f);
    loadingSprite.setPosition(WINDOW_WIDTH / 10.f, WINDOW_HEIGHT / 1.25f);

    BGObject* bgObject = new BGObject("BGObject");
    GameObjectManager::getInstance()->addObject(bgObject);
    TextureDisplay* display = new TextureDisplay();
    GameObjectManager::getInstance()->addObject(display);
    FPSCounter* fpsCounter = new FPSCounter();
    GameObjectManager::getInstance()->addObject(fpsCounter);
}

void BaseRunner::run() {
    sf::Clock clock; sf::Time acc = sf::Time::Zero;
    while (window.isOpen()) {
        sf::Time frame = clock.restart(); acc += frame;
        while (acc > TIME_PER_FRAME) { acc -= TIME_PER_FRAME; processEvents(); update(TIME_PER_FRAME); }
        render();
    }
}

void BaseRunner::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        else if (event.type == sf::Event::Resized) {
            sf::View view(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
            window.setView(view);
        }
        GameObjectManager::getInstance()->processInput(event);
    }
}

void BaseRunner::update(sf::Time elapsedTime) {
    GameObjectManager::getInstance()->update(elapsedTime);

    // Rotate spinner only during initial loading
    if (bgPhase == BGPhase::Splash && isLoading) {
        loadingSprite.rotate(3);
    }

    // Start fade exactly once after loading completes
    if (bgPhase == BGPhase::Splash && !isLoading) {
        bgPhase = BGPhase::FadingOut;
        isFading = true;
        fadeAlpha = 255.0f;
        fadeElapsedMs = 0.0f;
    }

    if (bgPhase == BGPhase::FadingOut) {
        fadeElapsedMs += elapsedTime.asMilliseconds();
        float t = std::min(fadeElapsedMs / FADE_DURATION_MS, 1.0f);
        fadeAlpha = 255.0f * (1.0f - t);
        loadingSprite.setColor(sf::Color(255,255,255,(sf::Uint8)fadeAlpha));
        if (t >= 1.0f) {
            // Fade finished
            isFading = false;
            bgPhase = BGPhase::Final; // BGObject will swap to Splash_1_0 once
            fadeAlpha = 0.0f; // no further alpha changes
        }
    }

    // Music crossfade unchanged
    if (!isLoading && !musicStarted && !isCrossfading && !musicFilePath.empty()) {
        if (backgroundMusic.openFromFile(musicFilePath)) {
            backgroundMusic.setLoop(true); backgroundMusic.setVolume(0); backgroundMusic.play();
            isCrossfading = true; crossfadeTime = 0.0f;
        }
    }
    if (isCrossfading) {
        crossfadeTime += elapsedTime.asSeconds();
        float progress = crossfadeTime / CROSSFADE_DURATION;
        if (progress >= 1.0f) {
            loadingMusic.stop(); backgroundMusic.setVolume(30); isCrossfading = false; musicStarted = true;
        } else {
            loadingMusic.setVolume(40.0f * (1.0f - progress));
            backgroundMusic.setVolume(50.0f * progress);
        }
    }
}

void BaseRunner::render() {
    window.clear();
    GameObjectManager::getInstance()->draw(&window);
    if (bgPhase != BGPhase::Final) {
        // draw spinner only before final phase
        window.draw(loadingSprite);
    }
    window.display();
}