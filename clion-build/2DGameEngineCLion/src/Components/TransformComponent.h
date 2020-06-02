#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#define EXTRA_CREDIT false

#include "../EntityManager.h"
#include "../../lib/glm/glm.hpp"
#include "SDL.h"
#include "../Game.h"
#include "../Constants.h"

#include <iostream>

class TransformComponent: public Component {
public:
	glm::vec2 position;
	glm::vec2 velocity;
	int width;
	int height;
	int scale;

	TransformComponent(int posX, int posY, int velX, int velY, int w, int h, int s) {
		position = glm::vec2(posX, posY);
		velocity = glm::vec2(velX, velY);
		width = w;
		height = h;
		scale = s;
	}

	void Initialize() override {

	}

	void Update(float deltaTime) override {
		// TODO
		// update the pos/vel as a function of deltaTime

#if EXTRA_CREDIT
		glm::vec2 new_pos;

		new_pos.x = position.x + (velocity.x * deltaTime);
		new_pos.y = position.y + velocity.y * deltaTime;
		
		if (new_pos.x > 0 && new_pos.x < WINDOW_WIDTH - 32) // hard coded in the choppers width
			position.x = new_pos.x;

		if (new_pos.y > 0 && new_pos.y < WINDOW_HEIGHT - 32) // hard coded in the choppers hieght
			position.y = new_pos.y;

#else
		position.x += velocity.x * deltaTime;
		position.y += velocity.y * deltaTime;
#endif
		
	}

	void Render() override {
		
	}

	std::string GetClassName() override{
		return "TransformComponent";
	}
};

#endif