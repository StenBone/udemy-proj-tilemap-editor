#include <iostream>
#include <fstream>
#include "./Constants.h"
#include "./Application.h"
#include "./AssetManager.h"
#include "./Map.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/ColliderComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Components/ProjectileEmitterComponent.h"
#include "../lib/glm/glm.hpp"
#include "Components/TileComponent.h"

EntityManager manager;
AssetManager* Application::assetManager = new AssetManager(&manager);
SDL_Renderer* Application::renderer;
SDL_Event Application::event;
SDL_Rect Application::camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Map* map;
TransformComponent* tileSelectorTransComp = NULL;
SpriteComponent* tilePreviewSpriteComp = NULL;
bool left_mouse_button_down = false;

struct Point2d {
    int x = -1, y = -1;
};

Entity* matrix[MATRIX_W][MATRIX_H]; // matrix to store map

Application::Application() {
	this->isRunning = false;
}

Application::~Application() {

}

bool Application::IsRunning() const {
	return this->isRunning;
}

void Application::Initialize(int width, int height) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "Error initializing SDL." << std::endl;
		return;
	}

	if (TTF_Init() != 0) {
		std::cerr << "Error initializing SDL TTF" << std::endl;
		return;
	}

	window = SDL_CreateWindow(
		TITLE.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
        SDL_WINDOW_SHOWN
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

void Application::LoadLevel(int levelNumber) {
	sol::state lua; //wrapper to lua state
	lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math); //libraries available to the lua script

	std::string levelName = "TilemapEditor";
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


    /**
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
	*/

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
            sol::optional<sol::table> textLabelCompExists = components["textLabel"];

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

			// TEXT LABEL COMP
            if (textLabelCompExists != sol::nullopt) {
                sol::table textLabelComp = components["textLabel"];
                int x = textLabelComp["x"];
                int y = textLabelComp["y"];
                std::string text = textLabelComp["text"];
                std::string fontFamily = textLabelComp["fontFamily"];

                // 	TextLabelComponent(int x, int y, std::string text, std::string fontFamily, const SDL_Color& color) {
                newEntity->AddComponent<TextLabelComponent>(x, y, text, fontFamily, WHITE_COLOR);
            }
		}
		entityIndex++;
	}
    tileSelectorTransComp = manager.GetEntityByName("tileSelector")->GetComponent<TransformComponent>();
    tilePreviewSpriteComp = manager.GetEntityByName("tilePreview")->GetComponent<SpriteComponent>();

    //manager.PrintEntitiesAndComponents();
}

void Application::ProcessInput() {
    // todo wasd moves map around
	SDL_PollEvent(&event);
	switch(event.type) {
		case SDL_QUIT: {
			isRunning = false;
			break;
		}

		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
				isRunning = false;
			}
			if (event.key.keysym.sym == SDLK_w) {
			    // write out a file with the information fromfrom matrix
			    // 00,00,
			    // saved_example.map
                std::ofstream myfile ("saved.map");
                if (myfile.is_open())
                {
                    for (int row = 0; row < MATRIX_H; row++) {
                        for (int col = 0; col < MATRIX_W; col++) {
                            Entity* ent = matrix[col][row];
                            if (ent != NULL)
                            {
                                std::string ent_name = ent->name;
                                std::string written_value = ent_name.substr(ent_name.find_first_of(':', 0) + 1, ent_name.length());
                                myfile << written_value;
                            } else{
                                myfile << "00";
                            }

                            if (col < MATRIX_W - 1) {
                                myfile << ",";
                            }


                        }
                        myfile << std::endl;
                    }
                }
                else std::cout << "Unable to open file";
			}
            break;
		}

		case SDL_MOUSEBUTTONDOWN: {

            switch (event.button.button) {
                case SDL_BUTTON_LEFT: {
                    left_mouse_button_down = true;
                    break;
                }
            }
            break;
		}

		case SDL_MOUSEBUTTONUP: {
		    switch (event.button.button) {
                case SDL_BUTTON_LEFT: {
                    left_mouse_button_down = false;
                    int x_coord = 0, y_coord = 0;
                    SDL_GetMouseState(&x_coord, &y_coord);
                    HandleLeftMouseButtonUp(x_coord, y_coord);
                    break;
                }
            }
            break;
		}

		default: {
			break;
		}
	}
}

void Application::HandleLeftMouseButtonDown(int x, int y) {

    // Is our click in the tileComponent field?
    // y value is on the edge of the frame
    if (x >= 0 && x <= MAP_PREVIEW_WIDTH &&
             y >= 0 && y < MAP_PREVIEW_HEIGHT) {

        // where to draw tileComp
        // snap to width of map preview
        // snap to height of map preview
        int snapX = x - x % TILE_SIZE;
        int snapY = y - y % TILE_SIZE;

        // matrix coordinates
        int mat_x = x / TILE_SIZE;
        int mat_y = y / TILE_SIZE;


        SDL_Rect srcRect = tilePreviewSpriteComp->GetSourceRectangle();
        // The ent_name is almost like a unique key.
        // The x and y are there because 11col0 could be translated x = 1 y = 10 or x = 11 y = 0
        std::string ent_name = "x" + std::to_string(mat_x) + "y" + std::to_string(mat_y) + ":" + std::to_string(srcRect.y / TILE_SIZE) + std::to_string(srcRect.x / TILE_SIZE);

        Entity* ent_at_mat = matrix[mat_x][mat_y];
        std::string name_from_mat = "";

        if (ent_at_mat != NULL)
        {
            name_from_mat = ent_at_mat->name;

            // does the selected tile already have the same tile?
            if (ent_name.compare(name_from_mat) == 0)
            {
                manager.DeleteEntityByName(ent_name);
            }
        }

        // draw a tile on the screen
        Entity& newTile(manager.AddEntity(ent_name, TILEMAP_LAYER));
        matrix[mat_x][mat_y] = &newTile; // returns pointer to what the ref is aliasing
        newTile.AddComponent<TileComponent>(srcRect.x, srcRect.y, snapX, snapY, TILE_SIZE, 1, "terrain-texture-day");
    }
}

void Application::HandleLeftMouseButtonUp(int x, int y) {
    // if x and y are over our tilemap image, print a message
    // sub coords from tilemap position
    // calculate source coords

    int tileSelectorX = tileSelectorTransComp->position.x;
    int tileSelectorW = tileSelectorTransComp->width + tileSelectorX;
    int tileSelectorY = tileSelectorTransComp->position.y;
    int tileSelectorH = tileSelectorTransComp->height + tileSelectorY;

    // Is our click on top of the tileSelector box?
    if (x >= tileSelectorX && x <= tileSelectorW &&
        y >= tileSelectorY && y <= tileSelectorH) {
        std::cout << "In box" << std::endl;

        // Offset the x and y by the tile selector position
        x = x - tileSelectorX;
        y = y - tileSelectorY;

        int snapX = x - x % TILE_SIZE;
        int snapY = y - y % TILE_SIZE;

        tilePreviewSpriteComp->SetSourceRectangle(snapX, snapY, TILE_SIZE, TILE_SIZE);
    }


}


// Application logic
void Application::Update() {

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


    if (left_mouse_button_down == true) {
        int x_coord = 0, y_coord = 0;
        SDL_GetMouseState(&x_coord, &y_coord);
        HandleLeftMouseButtonDown(x_coord, y_coord);
    }

}


void Application::Render() {
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

void Application::HandleCameraMovement() {
/**
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
 */
}

void Application::CheckCollisions() {

	CollisionType collisionType = manager.CheckCollisions();

	if (collisionType == PLAYER_ENEMY_COLLISION) {
		ProcessGameOver();
	}
	if (collisionType == PLAYER_PROJECTILE_COLLISION) {
		ProcessGameOver();
	}

	/**
	std::string collisionTagType = manager.CheckEntityCollisions(player);
	if (collisionTagType.compare("ENEMY") == 0) {
		// todo: do something when collision is identified with an enemy
		isRunning = false;
	} */
}

void Application::ProcessGameOver() {
	std::cout << "Application Over" << std::endl;
	isRunning = false;
}

void Application::Destroy() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
	