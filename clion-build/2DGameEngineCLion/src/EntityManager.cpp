#include "./EntityManager.h"
#include <iostream>
#include "./Collision.h"
#include "./Components/ColliderComponent.h"

void EntityManager::ClearData() {
	for (auto& entity: entities) {
		entity->Destroy();
	}
}

bool EntityManager::HasNoEntities() {
	return entities.size() == 0;
}

void EntityManager::Update(float deltaTime) {

	// todo use iterators for your loops and remember the note from lecture 41 about erasing elements and using iterators

	// EC Combine update and destroy loops
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->IsActive()) {
			entities[i]->Update(deltaTime);
		}
		else
		{
			entities.erase(entities.begin() + i); // remove entity from vector
		}
	}
	//DestroyInactiveEntities(); EC
}

void EntityManager::DestroyInactiveEntities() {
	for (int i = 0; i < entities.size(); i++) {
		if (!entities[i]->IsActive()) {
			entities.erase(entities.begin() + i); // remove entity from vector
		}
	}
}

void EntityManager::Render() {

	for (int layerNumber = 0; layerNumber < NUM_LAYERS; layerNumber++) {
		for (auto& entity: GetEntitiesByLayer(static_cast<LayerType>(layerNumber))) {
			entity->Render();
		}
	}

/*
	for (auto& entity: entities) {
		entity->Render();
	}*/
}

std::vector<Entity*> EntityManager::GetEntities() const {
	return entities;
} 

Entity* EntityManager::GetEntityByName(std::string name) const {
	for (auto* entity: entities) {
		if (entity->name.compare(name) == 0) {
			return entity;
		}
	}
	std::cerr << "No entity found" << std::endl;
	return NULL;
}

std::vector<Entity*> EntityManager::GetEntitiesByLayer(LayerType layer) const {
	std::vector<Entity*> selectedEntities;

	for (auto& entity: entities) {
		if (entity->layer == layer) {
			selectedEntities.emplace_back(entity);
		}
	}
	return selectedEntities;
}

unsigned int EntityManager::GetEntityCount() {
	return entities.size();
}

void EntityManager::PrintEntitiesAndComponents() const {

/*
Entity Name: projectileA
	Component<TransformComponent>
*/
	unsigned int i = 0;

	for (Entity* ent: entities)
	{
		std::cout << "Entity[" << i << "]: " << ent->name << std::endl;
		ent->PrintComponents();
		std::cout << std::endl;
		i++;
	}
	
}

CollisionType EntityManager::CheckCollisions() const {
	
	// TODO: how to optimize check further. Look at Rick's answer on Lecture 38
	for (int i = 0; i < entities.size() - 1; i++) {
		auto& thisEntity = entities[i]; 
		if (thisEntity->HasComponent<ColliderComponent>()) {
			ColliderComponent* thisCollider = thisEntity->GetComponent<ColliderComponent>();
			// optimized for that prevents checking for the same collision twice which was common in the previous double for strategy
			for (int j = i + 1; j < entities.size(); j++) {
				auto& thatEntity = entities[j];
				if (thisEntity->name.compare(thatEntity->name) != 0 && thatEntity->HasComponent<ColliderComponent>()) {
					ColliderComponent* thatCollider = thatEntity->GetComponent<ColliderComponent>();
					if (Collision::CheckRectangleCollision(thisCollider->collider, thatCollider->collider)) {
						if (
							thisCollider->colliderTag.compare("PLAYER") == 0 &&
							thatCollider->colliderTag.compare("ENEMY") == 0) 
						{
							return PLAYER_ENEMY_COLLISION;
						}

						if (
							thisCollider->colliderTag.compare("PLAYER") == 0 &&
							thatCollider->colliderTag.compare("PROJECTILE") == 0) 
						{
							return PLAYER_PROJECTILE_COLLISION;
						}

						if (
							thisCollider->colliderTag.compare("ENEMY") == 0 &&
							thatCollider->colliderTag.compare("FRIENDLY_PROJECTILE") == 0) 
						{
							return ENEMY_PROJECTILE_COLLISION;
						}

						// TODO: WOULD IT HELP PERFORMANCE TO HAVE THE MOST LIKELY COLLISIONS AT THE TOP AND LEAST LIKELY AT THE BOTTOM
						if (
							thisCollider->colliderTag.compare("PLAYER") == 0 &&
							thatCollider->colliderTag.compare("LEVEL_COMPLETE") == 0) 
						{
							return PLAYER_LEVEL_COMPLETE_COLLISION;
						}
					}
				}
			}
		}
	}
	return NO_COLLISION;
}

Entity& EntityManager::AddEntity(std::string entityName, LayerType layer) {
	Entity *entity = new Entity(*this, entityName, layer);
	entities.emplace_back(entity);
	return *entity;
}

bool EntityManager::DeleteEntityByName(std::string name) {

    std::vector<Entity*>::iterator it = entities.begin();
    for (it = entities.begin(); it != entities.end(); it++) {

        if ((*it)->name.compare(name) == 0) {
            entities.erase(it);
            return true;
        }
    }

    std::cerr << "No entity found" << std::endl;
    return false;
}

/** OBSOLETE
std::string EntityManager::CheckEntityCollisions(Entity& myEntity) const {
	ColliderComponent* myCollider = myEntity.GetComponent<ColliderComponent>();
	for (auto& entity: entities) {
		if(entity->name.compare(myEntity.name) != 0 && entity->name.compare("Tile") != 0) {
			if (entity->HasComponent<ColliderComponent>()) {
				ColliderComponent* otherCollider = entity->GetComponent<ColliderComponent>();

				if (Collision::CheckRectangleCollision(myCollider->collider, otherCollider->collider)) {
					return otherCollider->colliderTag;
				}
			}
		}
	}
	return std::string(); //empty string
}
*/