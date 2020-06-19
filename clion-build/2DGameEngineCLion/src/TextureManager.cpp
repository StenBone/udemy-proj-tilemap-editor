#include "./TextureManager.h"

SDL_Texture* TextureManager::LoadTexture(const char* fileName) {
	SDL_Surface* surface = IMG_Load(fileName);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Application::renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}


// The warnings of today are the errors of tomorrow
void TextureManager::Draw(SDL_Texture* texture, SDL_Rect sourceRectangle, SDL_Rect destinationRectangle, SDL_RendererFlip flip) {
	SDL_RenderCopyEx(Application::renderer, texture, &sourceRectangle, &destinationRectangle, 0.0, NULL, flip);

}