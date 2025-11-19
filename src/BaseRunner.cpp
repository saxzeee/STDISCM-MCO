#include "BaseRunner.h"
#include  "GameObjectManager.h"
#include "BGObject.h"
#include "TextureManager.h"
#include "TextureDisplay.h"
#include "FPSCounter.h"
#include <iostream>
#include <algorithm>

const sf::Time BaseRunner::TIME_PER_FRAME = sf::seconds(1.f / 60.f);

BaseRunner::BaseRunner() :
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "PS2: Interactive Loading Screen", sf::Style::Default) {
    this->window.setFramerateLimit(60);

    TextureManager::getInstance()->loadFromAssetList();

    if (loadingMusic.openFromFile("Media/Terraria_Title_OST.mp3")) {
        loadingMusic.setLoop(true);
        loadingMusic.setVolume(15);
        loadingMusic.play();
        loadingMusicStarted = true;
    }

    if (!loadingTexture.loadFromFile("Media/loading.png")) {
        std::cout << "[BaseRunner] Failed to load loading.png" << std::endl;
    }
    loadingSprite.setTexture(loadingTexture);
    loadingSprite.setOrigin(loadingTexture.getSize().x / 2.f, loadingTexture.getSize().y / 2.f);
    float maxDisplayWidth = 700.f;
    float currentWidth = static_cast<float>(loadingTexture.getSize().x);
    float scale = (currentWidth > maxDisplayWidth) ? (maxDisplayWidth / currentWidth) : 1.f;
    loadingSprite.setScale(scale, scale);

    float displayedWidth = loadingTexture.getSize().x * loadingSprite.getScale().x;
    loadingSprite.setPosition(displayedWidth / 2.f + 60.f, WINDOW_HEIGHT / 1.2f);

    configureProgressBarGeometry();

    BGObject* bgObject = new BGObject("BGObject");
    GameObjectManager::getInstance()->addObject(bgObject);
    TextureDisplay* display = new TextureDisplay();
    GameObjectManager::getInstance()->addObject(display);
    FPSCounter* fpsCounter = new FPSCounter();
    GameObjectManager::getInstance()->addObject(fpsCounter);
}

void BaseRunner::configureProgressBarGeometry() {
    sf::FloatRect bounds = loadingSprite.getGlobalBounds();

    float leftTrim = 20.0f;
    float rightTrim =25.0f;
    barInnerWidth = bounds.width - leftTrim - rightTrim;
    barInnerLeftOffset = leftTrim;

    barInnerHeight = bounds.height * 0.25f;// change for progress bar height and position
    float trackCenterY = bounds.height * 0.62f;
    barInnerTopOffset = trackCenterY - (barInnerHeight * 0.3f);// change for progress bar height and position

    progressFill.setPosition(bounds.left + barInnerLeftOffset, bounds.top + barInnerTopOffset);
    progressFill.setSize({0.f, barInnerHeight});
    progressFill.setFillColor(sf::Color::Transparent);
}

sf::Color BaseRunner::progressColorFor(float t) const {
    // Purple -> green gradient
    t = std::clamp(t, 0.f, 1.f);
    int rStart = 110, gStart = 0, bStart = 180;
    int rEnd = 0, gEnd = 230, bEnd = 70;
    int r = static_cast<int>(rStart * (1.f - t) + rEnd * t);
    int g = static_cast<int>(gStart * (1.f - t) + gEnd * t);
    int b = static_cast<int>(bStart * (1.f - t) + bEnd * t);
    return sf::Color(r,g,b);
}

void BaseRunner::updateProgressBar() {
    if (totalAssets == 0) return;
    float t = static_cast<float>(assetsLoaded) / static_cast<float>(totalAssets);
    t = std::clamp(t, 0.f, 1.f);
    progressFill.setSize({barInnerWidth * t, barInnerHeight});
    progressFill.setFillColor(progressColorFor(t));
    // Re-align in case of window resize
    sf::FloatRect bounds = loadingSprite.getGlobalBounds();
    progressFill.setPosition(bounds.left + barInnerLeftOffset, bounds.top + barInnerTopOffset);
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
            float displayedWidth = loadingTexture.getSize().x * loadingSprite.getScale().x;
            loadingSprite.setPosition(displayedWidth / 2.f + 60.f, event.size.height / 1.2f);
            configureProgressBarGeometry();
        }
        GameObjectManager::getInstance()->processInput(event);
    }
}

void BaseRunner::update(sf::Time elapsedTime) {
    GameObjectManager::getInstance()->update(elapsedTime);
    updateProgressBar();

    if (bgPhase == BGPhase::Splash && !isLoading) {
        bgPhase = BGPhase::HoldAfterLoad; // enter hold phase after loading finishes
        holdElapsedMs = 0.0f;
    }
    if (bgPhase == BGPhase::HoldAfterLoad) {
        holdElapsedMs += elapsedTime.asMilliseconds();
        if (holdElapsedMs >= postLoadHoldMs) {
            bgPhase = BGPhase::FadingOut;
            isFading = true;
            fadeAlpha = 255.0f;
            fadeElapsedMs = 0.0f;
        }
    }

    if (bgPhase == BGPhase::FadingOut) {
        fadeElapsedMs += elapsedTime.asMilliseconds();
        float t = std::min(fadeElapsedMs / FADE_DURATION_MS, 1.0f);
        fadeAlpha = 255.0f * (1.0f - t);
        loadingSprite.setColor(sf::Color(255,255,255,(sf::Uint8)fadeAlpha));
        auto pc = progressFill.getFillColor();
        progressFill.setFillColor(sf::Color(pc.r, pc.g, pc.b, (sf::Uint8)fadeAlpha));
        if (t >= 1.0f) {
            isFading = false;
            bgPhase = BGPhase::Final;
            fadeAlpha = 0.0f;
        }
    }

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
        window.draw(progressFill);
        window.draw(loadingSprite);
    }
    window.display();
}