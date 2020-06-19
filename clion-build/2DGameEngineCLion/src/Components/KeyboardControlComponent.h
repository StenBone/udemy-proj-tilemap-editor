#ifndef KEYBOARDCONTROLCOMPONENT_H
#define KEYBOARDCONTROLCOMPONENT_H

#include "../Application.h"
#include "../EntityManager.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

class KeyboardControlComponent: public Component {
public:
	std::string upKey;
	std::string downKey;
	std::string rightKey;
	std::string leftKey;
	std::string shootKey;

	TransformComponent *transform;
	SpriteComponent *sprite;

	KeyboardControlComponent() {

	}

	KeyboardControlComponent(std::string upKey, std::string rightKey, std::string downKey, std::string leftKey, std::string shootKey) {
		this->upKey = GetSDLKeyStringCode(upKey);
		this->rightKey = GetSDLKeyStringCode(rightKey);
		this->downKey = GetSDLKeyStringCode(downKey);
		this->leftKey = GetSDLKeyStringCode(leftKey);
		this->shootKey = GetSDLKeyStringCode(shootKey);

	}

	std::string GetSDLKeyStringCode(std::string key) {
		if (key.compare("up") == 0)    return "1073741906";
		if (key.compare("down") == 0)  return "1073741905";
		if (key.compare("left") == 0)  return "1073741904";
		if (key.compare("right") == 0) return "1073741903";
		if (key.compare("space") == 0) return "32";
		return std::to_string(static_cast<int>(key[0])); //returns ascii code of table
	}

	void Initialize() override {
		transform = owner->GetComponent<TransformComponent>();
		sprite = owner->GetComponent<SpriteComponent>();
	}

	void Update(float deltaTime) override {
		if (Application::event.type == SDL_KEYDOWN) {
			std::string keyCode = std::to_string(Application::event.key.keysym.sym);
			
			if (keyCode.compare(upKey) == 0) {
				transform->velocity.y = -25;
				transform->velocity.x = 0;
				sprite->Play("UpAnimation");
			}
			if (keyCode.compare(rightKey) == 0) {
				transform->velocity.y = 0;
				transform->velocity.x = 25;
				sprite->Play("RightAnimation");
			}
			if (keyCode.compare(downKey) == 0) {
				transform->velocity.y = 25;
				transform->velocity.x = 0;
				sprite->Play("DownAnimation");
			}
			if (keyCode.compare(leftKey) == 0) {
				transform->velocity.y = 0;
				transform->velocity.x = -25;
				sprite->Play("LeftAnimation");			
			}
			if (keyCode.compare(shootKey) == 0) {
				//todo
				//shoot projectiles when shoot key is pressed
			}

		}

		if (Application::event.type == SDL_KEYUP) {
			std::string keyCode = std::to_string(Application::event.key.keysym.sym);
			
			if (keyCode.compare(upKey) == 0) {
				transform->velocity.y = 0;
			}
			if (keyCode.compare(rightKey) == 0) {
				transform->velocity.x = 0;
			}
			if (keyCode.compare(downKey) == 0) {
				transform->velocity.y = 0;
			}
			if (keyCode.compare(leftKey) == 0) {
				transform->velocity.x = 0;
			}
		}
	}
};

#endif