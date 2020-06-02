#ifndef COLLIDERCOMPONENT_H
#define COLLIDERCOMPONENT_H

#define DEBUG_RENDER_COLLIDER false

#include <SDL2/SDL.h>
#include "../TextureManager.h"
#include "../AssetManager.h"
#include "../Game.h"
#include "../EntityManager.h"
#include "./TransformComponent.h"

class ColliderComponent : public Component {
public:
	std::string colliderTag;
	SDL_Rect collider; // dimensions of collider
	SDL_Rect sourceRectangle;
	SDL_Rect destinationRectangle;
	TransformComponent* transform;

	SDL_Texture* debugTexture; //EC

	ColliderComponent(std::string colliderTag, int x, int y, int width, int height) {
		this->colliderTag = colliderTag;
		this->collider = {x, y, width, height};
	}

	void Initialize() override {
		if(owner->HasComponent<TransformComponent>()) {
			transform = owner->GetComponent<TransformComponent>();
			sourceRectangle = {0, 0, transform->width, transform->height};
			destinationRectangle = {collider.x, collider.y, collider.w, collider.h};
		}

#if DEBUG_RENDER_COLLIDER
		debugTexture = Game::assetManager->GetTexture("debug-image"); //EC
#endif
	}

	void Update(float deltaTime) override {
		collider.x = static_cast<int>(transform->position.x); // todo is a cast needed here? testing
		collider.y = static_cast<int>(transform->position.y);
		collider.w = transform->width * transform->scale; //todo scale should already be applied or a method which passes it already applied
		collider.h = transform->height * transform->scale;
	
		destinationRectangle.x = collider.x - Game::camera.x;
		destinationRectangle.y = collider.y - Game::camera.y;

	}

	/** EXTRA CREDIT
		Render the bounding box of the ColliderComponent as an image
	*/
	void Render() override {

#if DEBUG_RENDER_COLLIDER
		TextureManager::Draw(debugTexture, sourceRectangle, destinationRectangle, SDL_FLIP_NONE);
#endif
	}

};

#endif