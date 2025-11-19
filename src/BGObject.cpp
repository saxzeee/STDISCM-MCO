#include "BGObject.h"
#include <iostream>
#include "TextureManager.h"
#include "BaseRunner.h"

BGObject::BGObject(string name) : AGameObject(name) {}

void BGObject::setBackgroundByName(const std::string& assetName) {
	sf::Texture* texture = TextureManager::getInstance()->getFromTextureMap(assetName, 0);
	if (texture) {
		this->sprite->setTexture(*texture, true);
		sf::Vector2u ts = texture->getSize();
		float scaleX = static_cast<float>(BaseRunner::WINDOW_WIDTH) / ts.x;
		float scaleY = static_cast<float>(BaseRunner::WINDOW_HEIGHT) / ts.y;
		this->sprite->setScale(scaleX, scaleY);
	}
}

void BGObject::initialize() {
	std::cout << "Declared as " << this->getName() << "\n";
	this->sprite = new sf::Sprite();
	setBackgroundByName("Splash_9_0");
	this->setPosition(0, 0);
	this->sprite->setColor(sf::Color(255,255,255,255));
}

void BGObject::processInput(sf::Event event) {}

void BGObject::update(sf::Time deltaTime) {
	// apply fading only during fading phase
	if (BaseRunner::bgPhase == BaseRunner::BGPhase::FadingOut) {
		this->sprite->setColor(sf::Color(255,255,255,(sf::Uint8)BaseRunner::fadeAlpha));
	}
	// switch background once on entering Final phase
	if (!switched && BaseRunner::bgPhase == BaseRunner::BGPhase::Final) {
		setBackgroundByName("Splash_1_0");
		this->sprite->setColor(sf::Color(255,255,255,255));
		switched = true;
		std::cout << "[BGObject] Switched to Splash_1_0" << std::endl;
	}
}
