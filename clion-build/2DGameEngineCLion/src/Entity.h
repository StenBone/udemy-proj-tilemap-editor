#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <string>
#include <map>
#include "./Constants.h"
#include "./EntityManager.h"
#include "./Component.h"
//#include "./Constants.h" // order of includes does matter. If this is below EntityManager, then EntityManager won't pick it up

class EntityManager;
class Component;

class Entity {
private:
	EntityManager& manager; // Already defined
	bool isActive;
	std::vector<Component*> components;
	std::map<const std::type_info*, Component*> componentTypeMap;
public: 
	std::string name;
	LayerType layer;
	Entity(EntityManager& manager);
	Entity(EntityManager& manager, std::string name, LayerType layer);
	// Has no pointers to destory, no need for destructor.
	void Update(float deltaTime);
	void Render();
	void Destroy();
	bool IsActive() const;
	void PrintComponents() const;

	template <typename T, typename... TArgs>
	T& AddComponent(TArgs&&... args) {
		T* newComponent(new T(std::forward<TArgs>(args)...));
		newComponent->owner = this;
		components.emplace_back(newComponent);
		componentTypeMap[&typeid(*newComponent)] = newComponent; // typeid ??? TODO: research more on references
		newComponent->Initialize();
		return *newComponent;
	}

	template <typename T>
	bool HasComponent() const {
		return componentTypeMap.count(&typeid(T));
	}

	template <typename T>
	T* GetComponent() {
		return static_cast<T*>(componentTypeMap[&typeid(T)]);
	}

	/** my verision
	template<typename T>
	bool HasComponent() const {
		// Returns true if our entity has component of type T
		if (componentTypeMap.count(&typeid(T)) > 0) {
			return true;
		}
		else {
			return false;
		}
	}*/
};



#endif