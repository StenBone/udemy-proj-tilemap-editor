#ifndef GAME_H
#define GAME_H

#include "SDL.h" //These are enviornment variable, hence the <>
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "../lib/lua/sol.hpp"
#include "./Entity.h"
#include "./Component.h"
#include "./EntityManager.h"

class AssetManager;

class Application {
	private:
		bool isRunning;
		SDL_Window *window;
	public:
		Application();
		~Application(); //Destructor
		int ticksLastFrame = 0;
		bool IsRunning() const; // Getter, const to ensure protection
		static SDL_Renderer *renderer;
		static AssetManager *assetManager;
		static SDL_Event event;
		static SDL_Rect camera;
		void LoadLevel(int levelNumber);
		void Initialize(int width, int height);
		void ProcessInput();
        void HandleLeftMouseButtonUp(int x, int y);
		void ProcessGameOver();
		void Update();
		void Render();
		void Destroy();
		void HandleCameraMovement();
		void CheckCollisions();

    void HandleLeftMouseButtonDown(int x, int y);
};

#endif 