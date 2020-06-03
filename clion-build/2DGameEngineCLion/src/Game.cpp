#include <iostream>
#include "./Constants.h"
#include "./Game.h"
#include "./AssetManager.h"
#include "./Map.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/ColliderComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Components/ProjectileEmitterComponent.h"
#include "../lib/glm/glm.hpp"

EntityManager manager;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Renderer* Game::renderer;
SDL_Event Game::event;
SDL_Rect Game::camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Entity* mainPlayer = NULL;
Map* map;

Game::Game() {
	this->isRunning = false;
}

Game::~Game() {

}

bool Game::IsRunning() const {
	return this->isRunning;
}

void Game::Initialize(int width, int height) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Error initializing SDL." << std::endl;
		return;
	}

	if (TTF_Init() != 0) {
		std::cerr << "Error initializing SDL TTF" << std::endl;
		return;
	}

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_BORDERLESS
		);
	// Just incase window didn't initialize correctly.
	if (!window) {
		std::cerr << "Error creating SDL window." << std::endl;
		return;
	}

	// -1 means to get the default driver
	// 0 means no flags
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		std::cerr << "Error creating SDL renderer." << std::endl;
		return;
	}

	// show colliders
	assetManager->AddTexture("debug-image", std::string("../assets/images/collision-texture.png").c_str());

	LoadLevel(1);
	//manager.PrintEntitiesAndComponents();


	isRunning = true;
	return;
}

void AddTransformCompToEntFromSol(sol::table transformComp, Entity* ent) {
	// TransformComponent(int posX, int posY, int velX, int velY, int w, int h, int s)	
	int posX = transformComp["position"]["x"];
	int posY = transformComp["position"]["y"];
	int velX = transformComp["velocity"]["x"];
	int velY = transformComp["velocity"]["y"];
	int width = transformComp["width"];
	int height = transformComp["height"];
	int scale = transformComp["scale"];
	int rot = transformComp["rotation"]; //not using this for the moment, only with the projectile emitter comp

    ent->AddComponent<TransformComponent>(posX, posY, velX, velY, width, height, scale);
}

void Game::LoadLevel(int levelNumber) {
	sol::state lua; //wrapper to lua state
	lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math); //libraries available to the lua script

	std::string levelName = "Level" + std::to_string(levelNumber);
	lua.script_file("../assets/scripts/" + levelName + ".lua");

    ///////////////////////////////////////////////////////////////////////////
    // LOAD LIST OF ASSETS FROM LUA CONFIG FILE
    ///////////////////////////////////////////////////////////////////////////
	sol::table levelData = lua[levelName];

	sol::table levelAssets = levelData["assets"];
	unsigned int assetIndex = 0;
	while(true) {
		sol::optional<sol::table> existsAssetIndexNode = levelAssets[assetIndex];
		if (existsAssetIndexNode == sol::nullopt) {
			break; //doesn't exists
		} else {
			sol::table asset = levelAssets[assetIndex];
			std::string assetType = asset["type"];
			if (assetType.compare("texture") == 0) {
				std::string assetId = asset["id"];
				std::string assetFile = asset["file"];
				assetManager->AddTexture(assetId, assetFile.c_str());
			}
            if (assetType.compare("font") == 0) {
                std::string assetId = asset["id"];
                std::string assetFile = asset["file"];
                int fontSize = asset["fontSize"];
                assetManager->AddFont(assetId, assetFile.c_str(), fontSize);
            }
		}
		assetIndex++;
	}

    ///////////////////////////////////////////////////////////////////////////
    // LOAD MAP AND TILE INFORMATION FROM LUA CONFIG FILE
    ///////////////////////////////////////////////////////////////////////////
	sol::table levelMap = levelData["map"];
	std::string mapTextureId = levelMap["textureAssetId"];
	std::string mapFile = levelMap["file"];

	map = new Map(
		mapTextureId,
		static_cast<int>(levelMap["scale"]),
		static_cast<int>(levelMap["tileSize"])
	);

	map->LoadMap (
		mapFile,
		static_cast<int>(levelMap["mapSizeX"]),
		static_cast<int>(levelMap["mapSizeY"])
	);

    ///////////////////////////////////////////////////////////////////////////
    // LOAD ENTITIES AND COMPONENTS FROM LUA CONFIG FILE
    ///////////////////////////////////////////////////////////////////////////
	sol::table levelEntities = levelData["entities"];
	unsigned int entityIndex = 0;
	Entity* newEntity; // entity that will be worked on in main loop
	while(true) {
		sol::optional<sol::table> existsEntityIndexNode = levelEntities[entityIndex];
		if (existsEntityIndexNode == sol::nullopt) {
			break; //doesn't exists
		} else {

			sol::table entityTable = levelEntities[entityIndex];
			std::string entityName = entityTable["name"];
			LayerType entityLayer = static_cast<LayerType>(entityTable["layer"]);

			newEntity = &(manager.AddEntity(entityName, entityLayer));

			// create variavle to check if components exists
			sol::table components = entityTable["components"];
			sol::optional<sol::table> transformCompExists = components["transform"];
			sol::optional<sol::table> spriteCompExists = components["sprite"];
			sol::optional<sol::table> colliderCompExists = components["collider"];
			sol::optional<sol::table> inputCompExists = components["input"]["keyboard"]; // for now only using keyboard
			sol::optional<sol::table> projectileEmitterExits = components["projectileEmitter"];

			// TRANSFORM COMP
			if (transformCompExists != sol::nullopt) {
				
					// TransformComponent(int posX, int posY, int velX, int velY, int w, int h, int s)	
					sol::table transformComp = components["transform"];

					int posX = transformComp["position"]["x"];
					int posY = transformComp["position"]["y"];
					int velX = transformComp["velocity"]["x"];
					int velY = transformComp["velocity"]["y"];
					int width = transformComp["width"];
					int height = transformComp["height"];
					int scale = transformComp["scale"];
					int rot = transformComp["rotation"]; //not using this for the moment, only with the projectile emitter comp

				    newEntity->AddComponent<TransformComponent>(posX, posY, velX, velY, width, height, scale);
			}

			// SPRITE COMP
			if (spriteCompExists != sol::nullopt) {
			    // 	SpriteComponent(std::string id, int numFrames, int animationSpeed, bool hasDirections, bool isFixed)
			    sol::table spriteComp = components["sprite"];
			    std::string textureAssetId = spriteComp["textureAssetId"];
			    bool animated = spriteComp["animated"];

			    // animated determines what constructor to call for the SpriteComponent
			    if (animated) {
			    	//	SpriteComponent(std::string id, int numFrames, int animationSpeed, bool hasDirections, bool isFixed) {
			    	int frameCount = spriteComp["frameCount"];
				    int animationSpeed = spriteComp["animationSpeed"];
				    bool hasDirections = spriteComp["hasDirections"];
				    bool fixed = spriteComp["fixed"];
			    	newEntity->AddComponent<SpriteComponent>(textureAssetId, frameCount, animationSpeed, hasDirections, fixed);	
			    }
			    else {
				    // SpriteComponent(const char* filePath) {
			    	newEntity->AddComponent<SpriteComponent>(textureAssetId.c_str());	
			    }
			}

			// INPUT COMP
			if (inputCompExists != sol::nullopt) {
				    //	KeyboardControlComponent(std::string upKey, std::string rightKey, std::string downKey, std::string leftKey, std::string shootKey) {
				    sol::table keyboardComp = components["input"]["keyboard"];
				    std::string up = keyboardComp["up"];
				    std::string right = keyboardComp["right"];
				    std::string down = keyboardComp["down"];
				    std::string left = keyboardComp["left"];
				    std::string shoot = keyboardComp["shoot"];
				    newEntity->AddComponent<KeyboardControlComponent>(up, right, down, left, shoot);
			}

		    // COLLIDER COMP
			if (colliderCompExists != sol::nullopt && transformCompExists != sol::nullopt) {
				// 	ColliderComponent(std::string colliderTag, int x, int y, int width, int height) {
				sol::table colliderComp = components["collider"];
				sol::table transformComp = components["transform"];

			    std::string tag = colliderComp["tag"]; // read transform comp to get values
				int posX = transformComp["position"]["x"];
				int posY = transformComp["position"]["y"];
				int width = transformComp["width"];
				int height = transformComp["height"];
			    newEntity->AddComponent<ColliderComponent>(tag, posX, posY, width, height);
			}

			// PROJECTILE EMITTER COMP
			// todo requires whole new entity
			if (projectileEmitterExits != sol::nullopt && transformCompExists != sol::nullopt) {
				// ProjectileEmitterComponent(int speed, int angleDeg, int range, bool should)
				// requires data of a parent entity
				sol::table transformComp = components["transform"];
				int parentEntityPosX = transformComp["position"]["x"];
				int parentEntityPosY = transformComp["position"]["y"];
				int parentEntityWidth = transformComp["width"];
				int parentEntityHeight = transformComp["height"];

				sol::table projectileEmitterComp = components["projectileEmitter"];
				int projectileSpeed = projectileEmitterComp["speed"];
				int projectileRange = projectileEmitterComp["range"];
				int projectileAngle = projectileEmitterComp["angle"];
				int projectileWidth = projectileEmitterComp["width"]; // not using
				int projectileHeight = projectileEmitterComp["height"]; // not using
				bool projectileShouldLoop = projectileEmitterComp["shouldLoop"];
				std::string textureAssetId = projectileEmitterComp["textureAssetId"]; // used earlier
				
				// we are creating a new entity on top of the existing one to be our projectile entity
				Entity& projectile(manager.AddEntity("projectile", PROJECTILE_LAYER));
				projectile.AddComponent<TransformComponent>(
					parentEntityPosX + (parentEntityWidth / 2),
					parentEntityPosY + (parentEntityHeight / 2),
					0,
					0,
					projectileWidth,
					projectileHeight,
					1
				);
				projectile.AddComponent<SpriteComponent>(textureAssetId);
				projectile.AddComponent<ProjectileEmitterComponent>(
					projectileSpeed,
					projectileAngle,
					projectileRange,
					projectileShouldLoop
				);
				projectile.AddComponent<ColliderComponent>(
					"PROJECTILE",
					parentEntityPosX,
					parentEntityPosY,
					parentEntityWidth,
					parentEntityHeight
				);
			}
			// TODO TextLabelComponent
		}
		entityIndex++;
	}

	mainPlayer = manager.GetEntityByName("player");

    //manager.PrintEntitiesAndComponents();
}

void Game::ProcessInput() {
	SDL_PollEvent(&event);
	switch(event.type) {
		case SDL_QUIT: {
			isRunning = false;
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
		}
		default: {
			break;
		}
	}
}

// Game logic
void Game::Update() {

	// Wait until 16ms has ellasped since the last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksLastFrame + FRAME_TARGET_TIME));

	// difference in ticks from last frame converted to seconds
	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f; // divided by 1000 to go from ms to seconds

	// Clamp deltaTime to a maximum value
	deltaTime = (deltaTime > 0.05f) ? 0.05f : deltaTime; // this is useful if you ever use a breakpoint

	// Sets the new ticks for the current frame to be used in the next pass
	ticksLastFrame = SDL_GetTicks(); // current time in ms since SDL_Init

	manager.Update(deltaTime); // update entities and components	

	HandleCameraMovement(); 
	CheckCollisions();
}


void Game::Render() {
	// Double buffer
	// One gets displayed while one gets drawn
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); // grey
	SDL_RenderClear(renderer); // clears back buffer

	if (manager.HasNoEntities()) {
		return;
	}

	manager.Render();

	SDL_RenderPresent(renderer); // swap buffers
}

void Game::HandleCameraMovement() {
	//todo no transform component sanity check
	if (mainPlayer) {
		TransformComponent* mainPlayerTransform = mainPlayer->GetComponent<TransformComponent>();
		
		camera.x = mainPlayerTransform->position.x - (WINDOW_WIDTH / 2);
		camera.y = mainPlayerTransform->position.y - (WINDOW_HEIGHT / 2);

		camera.x = camera.x < 0 ? 0 : camera.x; // clamp x above 0
		camera.y = camera.y < 0 ? 0 : camera.y;
		camera.x = camera.x > camera.w ? camera.w : camera.x; // clamp x below camera.w(WINDOW_WIDTH)
		camera.y = camera.y > camera.h ? camera.h : camera.y;
	}
}

void Game::CheckCollisions() {

	CollisionType collisionType = manager.CheckCollisions();

	if (collisionType == PLAYER_ENEMY_COLLISION) {
		ProcessGameOver();
	}
	if (collisionType == PLAYER_PROJECTILE_COLLISION) {
		ProcessGameOver();
	}
	if (collisionType == PLAYER_LEVEL_COMPLETE_COLLISION) {
		ProcessNextLevel(1);
	}

	/**
	std::string collisionTagType = manager.CheckEntityCollisions(player);
	if (collisionTagType.compare("ENEMY") == 0) {
		// todo: do something when collision is identified with an enemy
		isRunning = false;
	} */
}

void Game::ProcessNextLevel(int levelNumber) {
	std::cout << "Next Level" << std::endl;
	isRunning = false;
}

void Game::ProcessGameOver() {
	std::cout << "Game Over" << std::endl;
	isRunning = false;
}

void Game::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
	