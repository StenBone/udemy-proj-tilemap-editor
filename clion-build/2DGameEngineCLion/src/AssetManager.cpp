#include "./AssetManager.h"

AssetManager::AssetManager(EntityManager* manager) : manager(manager) {

}

void AssetManager::ClearData() {
	textures.clear();
	fonts.clear();
}

void AssetManager::AddTexture(std::string textureID, const char* filePath) {
	textures.emplace(textureID, TextureManager::LoadTexture(filePath));
}

void AssetManager::AddFont(std::string fontId, const char* filePath, int fontSize) {
	fonts.emplace(fontId, FontManager::LoadFont(filePath, fontSize));
}

SDL_Texture* AssetManager::GetTexture(std::string textureID) {
	return textures[textureID];
} 

TTF_Font* AssetManager::GetFont(std::string fontId) {
	return fonts[fontId];
}