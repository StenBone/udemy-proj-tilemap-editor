#include <iostream>
#include "./Constants.h"
#include "./Application.h"

int main(int argc, char *args[]) {
	Application *game = new Application();

	game->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

	while(game->IsRunning()) {
		game->ProcessInput();
		game->Update();
		game->Render();
	}

	game->Destroy();

	return 0;
}