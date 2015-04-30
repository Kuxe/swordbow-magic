#include "rendersystem.h"
#include "componentmanager.h"
#include "rendercomponent.h"
#include "movecomponent.h"
#include "sizecomponent.h"
#include <iostream>
#include "spatialindexer.h"
#include "heap.h"
#include "dynamicarray.h"
#include "heapsort.h"
#include <queue>
#include "namecomponent.h"
#include "systemmanager.h"
#include "hashgridsystem.h"

using namespace std;

bool SortHelper::operator< (const SortHelper& rhs) const {
    return
        renderComponent->zindex_base == rhs.renderComponent->zindex_base ?
        renderComponent->zindex < rhs.renderComponent->zindex :
        renderComponent->zindex_base < rhs.renderComponent->zindex_base;
}

bool SortHelper::operator> (const SortHelper& rhs) const { return rhs < *this; }
bool SortHelper::operator<=(const SortHelper& rhs) const { return !(*this > rhs); }
bool SortHelper::operator>=(const SortHelper& rhs) const { return !(*this < rhs); }


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
            cout << "ERROR: Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
            if(!renderer) {
                cout << "ERROR: Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
            } else {

				//imgFlags is used to initialize PNG-loading
				//so that IMG_Load can be used instead of SDL_LoadBMP
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
					cout << "ERROR: Couldnt initialze PNG usage!" << endl;
				} else {


		            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

			        targetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 8192, 8192);
					if(!targetTexture) {
						cout << "ERROR: Couldn't create targetTexture!" << endl;
						cout << "Screen will probably go black from now on" << endl;
						cout << "SDL_GetError(): " << SDL_GetError() << endl;
					}
					SDL_SetRenderTarget(renderer, targetTexture);
					SDL_RenderClear(renderer);
				}
            }
        }
    }

    //We want to access surfaces by their image path later on, so store paths right away
    for(auto path :{
            "./resources/images/HelloWorld.bmp",
            "./resources/images/player.bmp",
            "./resources/images/grass.bmp",
			"./resources/images/SmallTree.png",
            }
        )
    {
        SDL_Surface* rawImage = IMG_Load(path);
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
                            (unsigned int)w,
                            (unsigned int)h
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
	ids.insert(id);
	activeIds.push(id);
}

void RenderSystem::remove(unsigned long long int* id) {
	ids.erase(id);
}

void RenderSystem::update() {

	//Bellow is an algorithm that for each unmarked entity, that should be rendered,
	//adds it to a priority-queue (pq) and a queue (q).
	//Aslong as q isn't empty, pop an entity (e) from q. For all entities overlapping e,
	//add them to the pq and q, finally mark the overlapping entities.

	//In other words this algorithm adds all entities that should be rendered to a list.
	//An entity should be rendered whenever it is overlapped/overlapping another entity
	//that should be rendered (one entity that is set to be rendered might cause a group
	//of nearby, pairwise, overlapping entities to be redrawn to in order to maintain correct
	//z-order

	unsigned int count = ids.size();
	bool marked[100000] {false}; //TODO: Make less arbitrary, somehow..
	queue<unsigned long long int*> q;
	heap<SortHelper> pq; //TODO: Use reference instead (no need for copying!)

	auto spatialIndexer = dynamic_cast<SpatialIndexer*>(systemManager->getSystem("TextureHashGridSystem"));

	//For all activeIds in rendersystem (henceforth activeIds will be referred
	//to as just ids)
	while(!activeIds.empty()) {
        auto id = activeIds.front(); activeIds.pop();
        auto rc = componentManager->renderComponents.at(id);
        rc->textureData = textureDatas.at(rc->imagePath);
        rc->doRender = true;
        rc->cliprect = {
            0, 0, rc->textureData.width, rc->textureData.height
        };
        calculateZIndex(id);
        pq.insert({id, rc});

        //Make sure all overlapping ids have their part in the intersection area
        //are redrawn
        unordered_set<ID> overlappings;
        spatialIndexer->overlaps(overlappings, id);
        for(auto otherId : overlappings) {
            SpatialIndexer::Rect intersectionArea;
            SpatialIndexer::Rect idbb;
            SpatialIndexer::Rect oidbb;
            cout << endl << componentManager->nameComponents.at(otherId)->name << endl;

            spatialIndexer->getBoundingBox(idbb, id);
            spatialIndexer->getBoundingBox(oidbb, otherId);

            spatialIndexer->getIntersectionArea(
                intersectionArea,
                idbb,
                oidbb
            );
            auto mc = componentManager->moveComponents.at(otherId);
            auto rc = componentManager->renderComponents.at(otherId);
            cout << "intersectionarea: {" << intersectionArea.x << ", " << intersectionArea.y << ", " << intersectionArea.w << ", " << intersectionArea.h << "}" << endl;
            rc->cliprect = {
                -mc->xpos + rc->xoffset + intersectionArea.x, //Adjust cliprect from world-space to texture-space
                -mc->ypos + rc->yoffset + intersectionArea.y, //Thats why there's minus :)
                intersectionArea.w,
                intersectionArea.h
            };

            cout << "texture_cliprect: {" << rc->cliprect.x << ", " << rc->cliprect.y << ", " << rc->cliprect.w << ", " << rc->cliprect.h << "}" << endl;

            calculateZIndex(otherId);
            pq.insert({otherId, componentManager->renderComponents.at(otherId)});
            previousRedraws.push({otherId, componentManager->renderComponents.at(otherId)});
        }
	}

	SDL_SetTextureBlendMode(targetTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, targetTexture);

	while(!pq.isEmpty()) {
		auto sortHelper = pq.poll();
		render(sortHelper.id);
	}

	const auto& cameraXpos = componentManager->moveComponents.at(cameraTarget)->xpos - SCREEN_WIDTH/2;
	const auto& cameraYpos = componentManager->moveComponents.at(cameraTarget)->ypos - SCREEN_HEIGHT/2;
	const SDL_Rect camera = {
		cameraXpos < 0 ? 0 : (int)cameraXpos,
		cameraYpos < 0 ? 0 : (int)cameraYpos,
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	};

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, targetTexture, &camera, NULL);
	SDL_RenderPresent(renderer);
}

unsigned int RenderSystem::count() const {
	return ids.size();
}

void RenderSystem::render(unsigned long long int* id) const {
    //render if component wants to be re-rendered
    auto rc = componentManager->renderComponents.at(id);
    TextureData textureData;
    SDL_Rect rect{
        (int)(componentManager->moveComponents.at(id)->xpos + rc->xoffset + rc->cliprect.x),
        (int)(componentManager->moveComponents.at(id)->ypos + rc->yoffset + rc->cliprect.y),
        (int)rc->cliprect.w,
        (int)rc->cliprect.h
    };
    SDL_RenderCopy(renderer, textureDatas.at(rc->imagePath).texture, &rc->cliprect, &rect);
}

const string RenderSystem::getIdentifier() const {
	return "RenderSystem";
}

void RenderSystem::calculateZIndex(unsigned long long int* id) {
	auto rc = componentManager->renderComponents.at(id);
	auto mc = componentManager->moveComponents.at(id);
	rc->zindex = mc->ypos + rc->textureData.height + rc->yoffset;
}

void RenderSystem::makeIdActive(unsigned long long int* id) {
	//Only make the id active if it is a member of rendersystem
	if(ids.find(id) != ids.end()) {
		activeIds.push(id);
	}
}

void RenderSystem::setCameraTarget(unsigned long long int* id) {
	cameraTarget = id;
}
