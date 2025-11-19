#pragma once
#include "AGameObject.h"
class BGObject : public AGameObject
{
	public: 
		BGObject(String name);
		void initialize() override;
		void processInput(sf::Event event) override;
		void update(sf::Time deltaTime) override;
	private:
		void setBackgroundByName(const std::string& assetName);
		bool switched = false;
};

