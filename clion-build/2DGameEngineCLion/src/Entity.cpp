#include "./Entity.h"
#include <typeinfo>
#include <iostream>

Entity::Entity(EntityManager& manager): manager(manager) {
	this->isActive = true;
}

Entity::Entity(EntityManager& manager, std::string name, LayerType layer): manager(manager), name(name), layer(layer) {
	this->isActive = true;
}

void Entity::Update(float deltaTime) {

	// update all the compenents
	for (auto& component: components) {
		component->Update(deltaTime);
	}
}

void Entity::Render() {
	//render all components
	for (auto& component: components) {
		component->Render();
	}
}

void Entity::Destroy() {
	this->isActive = false;
}

bool Entity::IsActive() const {
	return this->isActive;
}

void Entity::PrintComponents() const {

	/**
	Component<TransformComponent>
	*/

	for (auto mapElement: componentTypeMap) {
		std::cout << "	Component<" << mapElement.first->name() << ">" << std::endl;
	}
}