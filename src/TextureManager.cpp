#include <fstream>
#include <iostream>
#include <filesystem>
#include "TextureManager.h"
#include "StringUtils.h"
#include "IETThread.h"

//a singleton class
TextureManager* TextureManager::sharedInstance = NULL;

TextureManager* TextureManager::getInstance() {
    if (sharedInstance == NULL) {
        //initialize
        sharedInstance = new TextureManager();
    }

    return sharedInstance;
}

TextureManager::TextureManager()
{
    this->countStreamingAssets();
}

void TextureManager::loadFromAssetList()
{
    std::cout << "[TextureManager] Reading from asset list" << std::endl;
    std::ifstream stream("Media/assets.txt");
    String path;

    while(std::getline(stream, path))
    {
        std::vector<String> tokens = StringUtils::split(path, '/');
        String assetName = StringUtils::split(tokens[tokens.size() - 1], '.')[0];
        this->instantiateAsTexture(path, assetName, false);
        std::cout << "[TextureManager] Loaded texture: " << assetName << std::endl;
    }
}

bool TextureManager::loadSingleStreamAsset(int index)
{
    int fileNum = 0;
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
        if(index == fileNum)
        {
            // simulate loading delay (optional): IETThread::sleep(50);

            const auto& pathObj = entry.path();
            String path = pathObj.string();
            // normalize separators to '/'
            for (auto& ch : path) { if (ch == '\\') ch = '/'; }
            String fileName = pathObj.filename().string();
            String assetName = StringUtils::split(fileName, '.')[0];

            this->instantiateAsTexture(path, assetName, true);
            std::cout << "[TextureManager] Loaded streaming texture: " << assetName << std::endl;
            return true;
        }
        fileNum++;
    }
    return false;
}

sf::Texture* TextureManager::getFromTextureMap(const String assetName, int frameIndex)
{
    if (!this->textureMap[assetName].empty()) {
        return this->textureMap[assetName][frameIndex];
    }
    else {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return NULL;
    }
}

int TextureManager::getNumFrames(const String assetName)
{
    if (!this->textureMap[assetName].empty()) {
        return (int)this->textureMap[assetName].size();
    }
    else {
        std::cout << "[TextureManager] No texture found for " << assetName << std::endl;
        return 0;
    }
}

sf::Texture* TextureManager::getStreamTextureFromList(const int index)
{
    if (index < 0 || index >= (int)this->streamTextureList.size()) return nullptr;
    return this->streamTextureList[index];
}

int TextureManager::getNumLoadedStreamTextures() const
{
    return (int)this->streamTextureList.size();
}

void TextureManager::countStreamingAssets()
{
    this->streamingAssetCount = 0;
    for (const auto& entry : std::filesystem::directory_iterator(STREAMING_PATH)) {
        (void)entry;
        this->streamingAssetCount++;
    }
    std::cout << "[TextureManager] Number of streaming assets: " << this->streamingAssetCount << std::endl;
}

void TextureManager::instantiateAsTexture(String path, String assetName, bool isStreaming)
{
    sf::Texture* texture = new sf::Texture();
    if (!texture->loadFromFile(path)) {
        std::cout << "[TextureManager] Failed to load texture from: " << path << std::endl;
    }
    this->textureMap[assetName].push_back(texture);

    if(isStreaming)
    {
        this->streamTextureList.push_back(texture);
    }
    else
    {
        this->baseTextureList.push_back(texture);
    }
    
}
