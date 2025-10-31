#include "FPSCounter.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include "BaseRunner.h"

FPSCounter::FPSCounter(): AGameObject("FPSCounter")
{
}

FPSCounter::~FPSCounter()
{
    delete this->statsText->getFont();
    delete this->statsText;
    AGameObject::~AGameObject();
}

void FPSCounter::initialize()
{
    sf::Font* font = new sf::Font();
    if (!font->loadFromFile("Media/Sansation.ttf"))
    {
        std::cout << "[FPSCounter] Failed to load font Media/Sansation.ttf" << std::endl;
    }

    this->statsText = new sf::Text();
    this->statsText->setFont(*font);
    this->statsText->setFillColor(sf::Color::White);
    this->statsText->setOutlineColor(sf::Color(0,0,0));
    this->statsText->setOutlineThickness(3.0f);
    this->statsText->setCharacterSize(35);
    this->statsText->setString("FPS: --.-");

    // place bottom-right with small margin using origin anchoring
    const float margin = 10.0f;
    auto bounds = this->statsText->getLocalBounds();
    this->statsText->setOrigin(bounds.left + bounds.width, bounds.top + bounds.height);
    this->statsText->setPosition(BaseRunner::WINDOW_WIDTH - margin, BaseRunner::WINDOW_HEIGHT - margin);

    updateTime = sf::Time::Zero;
    framesPassed = 0;
}

void FPSCounter::processInput(sf::Event event)
{
    if (event.type == sf::Event::Resized && this->statsText)
    {
        // Re-anchor to bottom-right when window size changes
        const float margin = 10.0f;
        auto bounds = this->statsText->getLocalBounds();
        this->statsText->setOrigin(bounds.left + bounds.width, bounds.top + bounds.height);
        this->statsText->setPosition(static_cast<float>(event.size.width) - margin, static_cast<float>(event.size.height) - margin);
    }
}

void FPSCounter::update(sf::Time deltaTime)
{
    this->updateFPS(deltaTime);
}

void FPSCounter::draw(sf::RenderWindow* targetWindow)
{
    AGameObject::draw(targetWindow);

    if(this->statsText != nullptr)
        targetWindow->draw(*this->statsText);
}

void FPSCounter::updateFPS(sf::Time elapsedTime)
{
    updateTime += elapsedTime;
    framesPassed++;

    // Update once per second for stable reading
    if (updateTime.asSeconds() >= 1.0f)
    {
        float fps = static_cast<float>(framesPassed) / updateTime.asSeconds();

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << "FPS: " << fps;
        this->statsText->setString(ss.str());

        // keep anchored to bottom-right as text width changes
        auto bounds = this->statsText->getLocalBounds();
        this->statsText->setOrigin(bounds.left + bounds.width, bounds.top + bounds.height);
        // Do not change position here; it is set initially and updated on resize

        updateTime -= sf::seconds(1.0f);
        framesPassed = 0;
    }
}
