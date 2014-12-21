#include "rendersystem.h"
#include "componentmanager.h"
#include "rendercomponent.h"
#include "movecomponent.h"
#include <iostream>

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

    //We want to access surfaces by their image path later on, so store paths right away
    for(auto a : {
            "./resources/images/HelloWorld.bmp",
            }
        ) 
    {
        SDL_Surface* surface = SDL_LoadBMP(a);
        if(!surface) {
            cout << "ERROR: Couldn't load image on: " << a << endl;
        } else {
            surfaces.insert(
                make_pair(
                    a,
                    surface
                )
            );
        }
    }
}

RenderSystem::~RenderSystem() {
    //Free all loaded images
    for(auto a : surfaces) {
        free(get<1>(a));
    }

	//Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}

void RenderSystem::add(unsigned long long int* id) {
	renderDatas.insert(
        make_pair(
            id,
            RenderData{
                componentManager->renderComponents.at(id),
                componentManager->moveComponents.at(id)
            }
        )
    );
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

void RenderSystem::render(const RenderData& data) const {
    cout << "rendering" << endl;

    SDL_Rect pos{
        (int)(data.moveComponent->xpos + data.renderComponent->xoffset),
        (int)(data.moveComponent->ypos + data.renderComponent->yoffset)
    };

    SDL_BlitSurface(
        surfaces.at(
            data.renderComponent->imagePath
        ), 
        NULL, 
        screenSurface, 
        &pos
    );
    SDL_UpdateWindowSurface(window);
}

const string RenderSystem::getIdentifier() const {
	return "RenderSystem";
}