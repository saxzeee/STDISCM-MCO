#pragma once
#include <unordered_map>
#include "SFML/Graphics.hpp"
#include <mutex>
#include <queue>
#include <memory>
#include <vector>

class TextureManager
{
public:
    typedef std::string String;
    typedef std::vector<sf::Texture*> TextureList;
    typedef std::unordered_map<String, TextureList> HashTable;
    struct PendingImage {
        String assetName;
        std::string path; // normalized path
    };
    struct DecodedImage {
        String assetName;
        std::shared_ptr<sf::Image> image; // decoded pixels
    };
    struct AudioAsset {
        String assetName;
        String filePath;
    };
    
public:
    static TextureManager* getInstance();
    void loadFromAssetList(); //loading of all assets needed for startup
    bool loadSingleStreamAsset(int index); //loads a single streaming asset based on index in directory, returns true if loaded
    sf::Texture* getFromTextureMap(const String assetName, int frameIndex);
    int getNumFrames(const String assetName);

    sf::Texture* getStreamTextureFromList(const int index);
    int getNumLoadedStreamTextures() const;

    // Thread-pool batching support
    void scheduleStreamingScan();
    bool popPendingImage(PendingImage& out);
    void pushPendingImage(PendingImage img);

    // Decoded (CPU) image ready queue
    void enumerateStreamingFiles(std::vector<std::string>& out) const;
    void pushReadyImage(DecodedImage img);
    bool popReadyImage(DecodedImage& out);
    bool registerReadyImageToTexture(const DecodedImage& img);
    // for audio
    void markAudioAsReady(const String& assetName, const String& path);
    bool popReadyAudio(AudioAsset& out);

private:
    TextureManager();
    TextureManager(TextureManager const&) {};             // copy constructor is private
    TextureManager& operator=(TextureManager const&) {};  // assignment operator is private
    static TextureManager* sharedInstance;

    HashTable textureMap;
    TextureList baseTextureList;
    TextureList streamTextureList;

    const std::string STREAMING_PATH = "Media/Streaming/";
    int streamingAssetCount = 0;

    void countStreamingAssets();
    void instantiateAsTexture(String path, String assetName, bool isStreaming);
    void instantiateFromImage(const String& assetName, const sf::Image& image, bool isStreaming);

    // legacy pending filenames queue
    mutable std::mutex m_pendingMutex;
    std::queue<PendingImage> m_pendingImages;

    // ready decoded images from worker threads
    mutable std::mutex m_readyMutex;
    std::queue<DecodedImage> m_readyImages;

    std::queue<AudioAsset> m_readyAudio;
    std::mutex m_audioMutex;
};