#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "SDL.h"
#include "../TextureManager.h"
#include "../AssetManager.h"
#include "../Animation.h"

class SpriteComponent : public Component {

private:
	TransformComponent* transform;
	SDL_Texture* texture;
	SDL_Rect sourceRectangle;
	SDL_Rect destinationRectangle;
	
	//Animation
	bool isAnimated;
	int numFrames; //number of frames in animation
	int animationSpeed;
	bool isFixed; //Fixed to a part of the screen, like a UI
	std::map<std::string, Animation> animations;
	std::string currentAnimationName;
	unsigned int animationIndex = 0;

public: 
	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent(const char* filePath) {
		isAnimated = false;
		isFixed = false;
		SetTexture(filePath);
	}

    SpriteComponent(std::string assetTextureId) {
        this->isAnimated = false;
        this->isFixed = false;
        SetTexture(assetTextureId);
    }

    SpriteComponent(std::string assetTextureId, bool isFixed) {
        this->isAnimated = false;
        this->isFixed = isFixed;
        SetTexture(assetTextureId);
    }

	SpriteComponent(std::string id, int numFrames, int animationSpeed, bool hasDirections, bool isFixed) {
		this->isAnimated = true;
		this->numFrames = numFrames;
		this->animationSpeed = animationSpeed;
		this->isFixed = isFixed;

		if (hasDirections)
		{
			Animation downAnimation = Animation(0, numFrames, animationSpeed);
			Animation rightAnimation = Animation(1, numFrames, animationSpeed);
			Animation leftAnimation = Animation(2, numFrames, animationSpeed);
			Animation upAnimation = Animation(3, numFrames, animationSpeed);

			animations.emplace("DownAnimation", downAnimation);
			animations.emplace("RightAnimation", rightAnimation);
			animations.emplace("LeftAnimation", leftAnimation);
			animations.emplace("UpAnimation", upAnimation);	
			this->animationIndex = 0;
			this->currentAnimationName = "DownAnimation";
		} else {
			Animation singleAnimation = Animation(0, numFrames, animationSpeed);
			animations.emplace("SingleAnimation", singleAnimation);
			this->animationIndex = 0;
			this->currentAnimationName = "SingleAnimation";
		}

		Play(this->currentAnimationName);

		SetTexture(id);
	}

	void Play(std::string animationName) {
		numFrames = animations[animationName].numFrames;
		animationIndex = animations[animationName].index;
		animationSpeed = animations[animationName].animationSpeed;
		currentAnimationName = animationName;
	}

	void SetTexture(std::string assetTextureID) {
		texture = Application::assetManager->GetTexture(assetTextureID);
	}

	void SetSourceRectangle(int x, int y, int w, int h) {
	    sourceRectangle.x = x;
	    sourceRectangle.y = y;
	    sourceRectangle.w = w;
	    sourceRectangle.h = h;
	}

	void Initialize() override {
		transform = owner->GetComponent<TransformComponent>();
		sourceRectangle.x = 0;
		sourceRectangle.y = 0;
		sourceRectangle.w = transform->width;
		sourceRectangle.h = transform->height;
	}

	void Update(float deltaTime) override {
		if (isAnimated) {
			// the rectangle is shifting over the sprite
			sourceRectangle.x = sourceRectangle.w * static_cast<int>((SDL_GetTicks() / animationSpeed) % numFrames);
            sourceRectangle.y = animationIndex * transform->height; // must be inside animation if
        }

		destinationRectangle.x = static_cast<int>(transform->position.x) - (isFixed ? 0 : Application::camera.x); //offset by camera position
		destinationRectangle.y = static_cast<int>(transform->position.y) - (isFixed ? 0 : Application::camera.y); //offset by camera position
		destinationRectangle.w = transform->width * transform->scale;
		destinationRectangle.h = transform->height * transform->scale;
	}

	void Render() override {
		TextureManager::Draw(texture, sourceRectangle, destinationRectangle, spriteFlip);
	}
};

#endif
