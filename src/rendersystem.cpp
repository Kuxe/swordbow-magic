#include "rendersystem.h"
#include "componentmanager.h"
#include "rendercomponent.h"
#include "movecomponent.h"
#include <iostream>
#include "gridindexer.h"
#include "flagcomponent.h"

using namespace std;

RenderSystem::RenderSystem(GridIndexer* gridIndexer)
    : gridIndexer(gridIndexer) {
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
            cout << "ERROR: Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
            if(!renderer) {
                cout << "ERROR: Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
            } else {
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

                targetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
				SDL_SetRenderTarget(renderer, targetTexture);
                SDL_RenderClear(renderer);
            }
        }
    }

    //We want to access surfaces by their image path later on, so store paths right away
    for(auto path :{
            "./resources/images/HelloWorld.bmp",
            "./resources/images/player.bmp",
            "./resources/images/grass.bmp",
            }
        ) 
    {
        SDL_Surface* rawImage = SDL_LoadBMP(path);
        if(!rawImage) {
            cout << "ERROR: Couldn't load image on: " << path << endl;
        } else {
            //Make purple parts of images transparent by using color keying
            SDL_SetColorKey(rawImage, SDL_TRUE, SDL_MapRGB( rawImage->format, 0xFF, 0x00, 0xFF ));
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, rawImage);
            if(!texture) {
                cout << "ERROR: Couldn't convert surface on addr " << rawImage << " to texture! SDL_Error: " << SDL_GetError() << endl;
            } else {
                SDL_FreeSurface(rawImage);
                int w, h;
                SDL_QueryTexture(texture, NULL, NULL, &w, &h);
                textureDatas.insert(
                    make_pair(
                        path,
                        TextureData{
                            texture,
                            w,
                            h
                        }
                    )
                );
            }
        }
    }
}

RenderSystem::~RenderSystem() {
    //Free all loaded images
    for(auto textureData : textureDatas) {
        SDL_DestroyTexture(get<1>(textureData).texture);
    }

    SDL_DestroyTexture(targetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
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
    for(int i = 0; i < MAX_IDS; i++) {
        if(ids[i] == 0) {
            ids[i] = id;
            i = MAX_IDS;
        }
    }
}

void RenderSystem::remove(unsigned long long int* id) {
	if(id == nullptr) {
		cout << "WARNING: No id on adress: " << id << endl;
		return;
	}
	if(renderDatas.erase(id) == 0) {
		cout << "WARNING: RenderSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
    for(int i = 0; i < MAX_IDS; i++) {
        if(ids[i] == id) {
            ids[i] = 0;
            return;
        }
    }
}

void RenderSystem::update() {
    //if any entity took an action which requires rendering, then nearby components
    //probably needs re-rendering too
    for(unsigned char i = 0; i < MAX_IDS; i++) {
        unsigned long long int* currentId = ids[i];
        if(currentId != 0 && componentManager->flagComponents.at(currentId)->flags & FlagComponent::HAS_CHANGED) {
            componentManager->renderComponents.at(currentId)->doRender = true;
            renderingRequired = true;
            for(auto closeIds : gridIndexer->getOverlappingIds(currentId)) {
                componentManager->renderComponents.at(closeIds)->doRender = true;
            }
        }
    }
    if(renderingRequired) {
        //Array of sorted renderdatas (by z-index)
        int i = 0;
        const int SIZE = renderDatas.size();
        RenderData sortedRenderDatas[SIZE];

        //Copy renderDatas unordered_map to an array
    	for(auto data : renderDatas) {
    		sortedRenderDatas[i] = get<1>(data);
            i++;
    	}

        //Sort the array by z-index
        bool sorted = true;
        while(!sorted) {
            sorted = true;
            for(i = 0; i < SIZE-1; i++) {
                const float z1 = sortedRenderDatas[i].renderComponent->zindex;
                const float z2 = sortedRenderDatas[i+1].renderComponent->zindex;

                if(z1 > z2) {
                    const auto temp = sortedRenderDatas[i];
                    sortedRenderDatas[i] = sortedRenderDatas[i+1];
                    sortedRenderDatas[i+1] = temp;
                    sorted = false;
                }
            }
        }
        for(auto data : sortedRenderDatas) {
            render(data);
        }

        //Set renderTarget to default, clear it, and render targetTexture to default. Finally render default.
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, targetTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
        renderingRequired = false;
    }
}

unsigned int RenderSystem::count() const {
	return renderDatas.size();
}

void RenderSystem::render(const RenderData& data) const {
    //render if component wants to be re-rendered
    if(data.renderComponent->doRender) {
        TextureData textureData;
        try {
            textureData = textureDatas.at(data.renderComponent->imagePath);
        } catch (const std::out_of_range &oor) {
            cout << "out_of_range exception caught in SystemManager::getSystem(string identifier): " << oor.what() << endl;
            cout << "1. Maybe there as a typo in the RenderComponent->imagePath, " << data.renderComponent->imagePath << "?" << endl;
            cout << "2. Maybe " << data.renderComponent->imagePath << " wasn't loaded by RenderSystem?" << endl;
            cout << "Things will go wrong from now on!" << endl;
        }

        SDL_Rect rect{
            (int)(data.moveComponent->xpos + data.renderComponent->xoffset),
            (int)(data.moveComponent->ypos + data.renderComponent->yoffset),
            textureData.width,
            textureData.height
        };

        //Render to target texture
        //the target texture in turn will be rendered ontop of default
        //the default is always cleared
        //SDL_RenderPresent invalidates current backbuffer, so hence this required
        //the targetTexture ISNT invalidated, whatever is rendered onto it it saved
        //so it's fine to just re-render the targetTexture
        SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, targetTexture);
        SDL_RenderCopy(renderer, textureDatas.at(data.renderComponent->imagePath).texture, NULL, &rect);

        //Finally, don't render this component until it says otherwise
        data.renderComponent->doRender = false;
    }
}

const string RenderSystem::getIdentifier() const {
	return "RenderSystem";
}
