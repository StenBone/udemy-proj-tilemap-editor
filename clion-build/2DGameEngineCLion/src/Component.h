#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

class Entity; //DELETE_ME temporary class

class Component {
public:
	Entity* owner;
	virtual ~Component() {} // Creating the functions inline to not need a class file
	virtual void Initialize() {}
	virtual void Update(float deltaTime) {}
	virtual void Render() {}
	virtual std::string GetClassName() {}
};



#endif