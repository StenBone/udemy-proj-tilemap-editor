#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "./Entity.h"
#include "./Component.h"
#include <vector>

class EntityManager {
private:
	std::vector<Entity*> entities;
public:
	void ClearData();
	void Update(float deltaTime);
	void Render();
	bool HasNoEntities();
	unsigned int GetEntityCount();
	void PrintEntitiesAndComponents() const;
	std::vector<Entity*> GetEntities() const;
	Entity* GetEntityByName(std::string name) const;
	std::vector<Entity*> GetEntitiesByLayer(LayerType layer) const;
	Entity& AddEntity(std::string entityName, LayerType layer);
	CollisionType CheckCollisions() const;
	void DestroyInactiveEntities();
	//std::string CheckEntityCollisions(Entity& entity) const;
};


#endif