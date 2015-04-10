#include "rendersystem.h"
#include "componentmanager.h"
#include "rendercomponent.h"
#include "movecomponent.h"
#include "sizecomponent.h"
#include <iostream>
#include "spatialindexer.h"
#include "flagcomponent.h"
#include "heap.h"
#include "dynamicarray.h"
#include "heapsort.h"
#include <queue>

using namespace std;




bool RenderData::operator< (const RenderData& rhs) const {
	//RenderData is first ordered by zindex_base and if the bases are equal then zindex
	//{zindex_base, zindex}
	//{6, 4.9} is greater than {5, 5.0}
	//{5, 5.0} is equal to {5, 5.0}
	//{5, 4.9] is less than {5, 5.0}
	if(renderComponent->zindex_base == rhs.renderComponent->zindex_base) {
		return renderComponent->zindex < rhs.renderComponent->zindex;
	} else {
		return renderComponent->zindex_base < rhs.renderComponent->zindex_base;
	}
}

bool RenderData::operator> (const RenderData& rhs) const { return rhs < *this; }
bool RenderData::operator<=(const RenderData& rhs) const { return !(*this > rhs); }
bool RenderData::operator>=(const RenderData& rhs) const { return !(*this < rhs); }


RenderSystem::RenderSystem(SpatialIndexer* spatialIndexer)
    : spatialIndexer(spatialIndexer) {
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

			        targetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
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
	renderDatas.insert(
        make_pair(
            id,
            RenderData{
                componentManager->renderComponents.at(id),
                componentManager->moveComponents.at(id),
				componentManager->sizeComponents.at(id),
				componentManager->flagComponents.at(id),
            }
        )
    );
	ids.add(id);
}

void RenderSystem::remove(unsigned long long int* id) {
	if(id == nullptr) {
		cout << "WARNING: No id on adress: " << id << endl;
		return;
	}
	if(renderDatas.erase(id) == 0) {
		cout << "WARNING: RenderSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
	ids.remove(id);
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

	unsigned int count = ids.getSize();
	bool marked[100000] {false}; //TODO: Make less arbitrary, somehow..
	queue<unsigned long long int*> q;
	heap<RenderData> pq; //TODO: Use reference instead (no need for copying!)

	//For all ids in rendersystem
	for(auto& id : ids){

		//Always make force textureData to be up-to-date with the image at rendercomponent
		auto& rc = componentManager->renderComponents.at(id);
		rc->textureData = textureDatas.at(rc->imagePath);

		//if all ids havnt been added already
		//and either HAS_CHANGED or doRender is true
		//and the current id hasnt been targeted for rendering
		//then add the id pq of renderdatas to be rendered
		//and put id in the list
		//finally mark it
		if(count != 0 && (componentManager->flagComponents.at(id)->flags & FlagComponent::HAS_CHANGED
				|| renderDatas.at(id).renderComponent->doRender)
				&& !marked[*id]) {

			q.push(id);
			pq.insert(renderDatas.at(id));
			marked[*id] = true;
            componentManager->renderComponents.at(id)->doRender = true;
			count--;

			//as long as list isnt empty and all ids havnt been added already
			//erase an id from the list and add overlapping ids into the pq of
			//renderdatas, and also put them in the list. Mark them.
			while(!q.empty() && count != 0) {
				unsigned long long int* cur = q.back(); q.pop();
				unordered_set<ID> curOverlaps;
				spatialIndexer->overlaps(curOverlaps, cur);
				for(auto overlap : curOverlaps) {
					if(!marked[*overlap] && count != 0) {
						q.push(overlap);
						pq.insert(renderDatas.at(overlap));
						marked[*overlap] = true;
						componentManager->renderComponents.at(overlap)->doRender = true;
						count--;
					}
				}
			}
		}
	}

	while(!pq.isEmpty()) {
		RenderData data = pq.poll();
		render(data);
	}

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, targetTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
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
            (int)textureData.width,
            (int)textureData.height
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

void RenderSystem::sort(RenderData* arr, const unsigned int size) const {
	//Sort the array by z-index

	//set zindex to be lowest y
	for(auto a : renderDatas) {
		get<1>(a).renderComponent->zindex = get<1>(a).moveComponent->ypos + get<1>(a).sizeComponent->height;
	}
	heapsort(arr, size);
}
