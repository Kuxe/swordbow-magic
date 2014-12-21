#include "rendersystem.h"
#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

RenderSystem::RenderSystem() {
	//Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "ERROR: SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
    } else {
        window = SDL_CreateWindow(
        	"SDL Tutorial", 
        	SDL_WINDOWPOS_UNDEFINED,
        	SDL_WINDOWPOS_UNDEFINED,
        	SCREEN_WIDTH,
        	SCREEN_HEIGHT,
        	SDL_WINDOW_SHOWN);

        if(!window) {
            cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        } else {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF));
            
            //Update the surface
            SDL_UpdateWindowSurface(window);
        }
    }
}

RenderSystem::~RenderSystem() {
	//Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}

void RenderSystem::add(unsigned long long int* id, RenderData data) {
	renderDatas.insert(make_pair(id, data));
}

void RenderSystem::remove(unsigned long long int* id) {
	if(id == nullptr) {
		cout << "WARNING: No id on adress: " << id << endl;
		return;
	}
	if(renderDatas.erase(id) == 0) {
		cout << "WARNING: RenderSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
}

void RenderSystem::update() {
	for(auto data : renderDatas) {
		render(get<1>(data));
	}
}

unsigned int RenderSystem::count() const {
	return renderDatas.size();
}

void RenderSystem::render(RenderData data) {
	//Dummy render-method
}

const string RenderSystem::getIdentifier() const {
	return "RenderSystem";
}