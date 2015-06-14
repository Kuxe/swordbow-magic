#include "rendersystem.hpp"
#include "componentmanager.hpp"
#include "rendercomponent.hpp"
#include "movecomponent.hpp"
#include "sizecomponent.hpp"
#include <iostream>
#include "spatialindexer.hpp"
#include "heap.hpp"
#include "dynamicarray.hpp"
#include "heapsort.hpp"
#include <queue>
#include "namecomponent.hpp"
#include "systemmanager.hpp"
#include "hashgridsystem.hpp"
#include "deltatime.hpp"
#include "camerasystem.hpp"

using namespace std;

bool RenderData::operator< (const RenderData& rhs) const {
    return
        renderComponent->zindex_base == rhs.renderComponent->zindex_base ?
        renderComponent->zindex < rhs.renderComponent->zindex :
        renderComponent->zindex_base < rhs.renderComponent->zindex_base;
}

bool RenderData::operator> (const RenderData& rhs) const { return rhs < *this; }
bool RenderData::operator<=(const RenderData& rhs) const { return !(*this > rhs); }
bool RenderData::operator>=(const RenderData& rhs) const { return !(*this < rhs); }


RenderSystem::RenderSystem() {
	//Initialize SDL
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
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

                    fontTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
                    if(!fontTexture) {
						cout << "ERROR: Couldn't create fontTexture!" << endl;
						cout << "Screen will probably go black from now on" << endl;
						cout << "SDL_GetError(): " << SDL_GetError() << endl;
					}
                    SDL_SetTextureBlendMode(fontTexture, SDL_BLENDMODE_BLEND);

			        worldTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 8192, 8192);
					if(!worldTexture) {
						cout << "ERROR: Couldn't create worldTexture!" << endl;
						cout << "Screen will probably go black from now on" << endl;
						cout << "SDL_GetError(): " << SDL_GetError() << endl;
					}
					SDL_SetRenderTarget(renderer, worldTexture);
					SDL_RenderClear(renderer);
				}

                if(TTF_Init() < 0 ) {
                    cout << "ERROR: Couldn't initialize SDL2_ttf!" << endl;
                } else {
                    font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 11);
                    if(!font) {
                        cout << "ERROR: Failed to load font!" << endl;
                    }
                }
            }
        }
    }

    //We want to access surfaces by their image path later on, so store paths right away
    for(auto path :{
            "./resources/images/HelloWorld.bmp",
            "./resources/images/player.bmp",
            "./resources/images/grass.png",
			"./resources/images/SmallTree.png",
            "./resources/images/playerv2.png",


            "./resources/images/playerv3_front.png",
            "./resources/images/playerv3_back.png",
            "./resources/images/playerv3_left.png",
            "./resources/images/playerv3_right.png",

            "./resources/images/playerv3_front_run1.png",
            "./resources/images/playerv3_front_run2.png",
            "./resources/images/playerv3_front_run3.png",
            "./resources/images/playerv3_front_run4.png",

            "./resources/images/bloodsplatter1_1.png",
            "./resources/images/bloodsplatter1_2.png",
            "./resources/images/bloodsplatter1_3.png",
            "./resources/images/bloodsplatter1_4.png",
            "./resources/images/bloodsplatter1_5.png",
            "./resources/images/bloodsplatter1_6.png",
            "./resources/images/bloodsplatter1_7.png",
            "./resources/images/bloodsplatter1_8.png",
            "./resources/images/bloodsplatter1_9.png",
            "./resources/images/bloodsplatter1_10.png",
            "./resources/images/bloodsplatter1_11.png",

            "./resources/images/yellow_flower.png",
            "./resources/images/green_flower.png",
            "./resources/images/violet_flower.png",
            "./resources/images/blue_flower.png",

            "./resources/images/testsquare10x10.png",
            "./resources/images/testsquare1x1.png",
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
    TTF_CloseFont(font);
    TTF_Quit();

    //Free all loaded images
    for(auto textureData : textureDatas) {
        SDL_DestroyTexture(get<1>(textureData).texture);
    }

    SDL_DestroyTexture(fontTexture);
    SDL_DestroyTexture(worldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void RenderSystem::add(ID id) {
	ids.insert(id);
	activeIds.push(id);
}

void RenderSystem::remove(ID id) {
	ids.erase(id);
}

void RenderSystem::update() {
	heap<RenderData> pq; //TODO: Use reference instead (no need for copying!)
	auto spatialIndexer = dynamic_cast<SpatialIndexer*>(systemManager->getSystem("TextureHashGridSystem"));

    //Will contain all rectangles where a redraw is required
    queue<Rect> drawQueue;

    //All drawareas saved from previous frame should be drawn this frame
	drawQueue.swap(previousDrawAreas);


    //Cap the amount of IDs getting rendered each frame
    //this is only a quick-fix to ensure no program-freeze
    //when initially rendering the whole world
    char allowedRendersThisFrame = 32;
	//For all activeIds in rendersystem...
	while(!activeIds.empty() && allowedRendersThisFrame-- > 0 ) {
		//Draw everything within the activeIds texturearea
        auto drawArea = spatialIndexer->getBoundingBox(activeIds.front());
		drawQueue.push(drawArea); activeIds.pop();

        //The texturearea should be saved for next frame, it must be redrawn then too
		renderArea(drawArea);
	}

    while(!drawQueue.empty()) {
        const auto& drawArea = drawQueue.front();

        //Make sure all overlapping ids have their part in the intersection area
    	//are redrawn
    	auto spatialIndexer = dynamic_cast<SpatialIndexer*>(systemManager->getSystem("TextureHashGridSystem"));

    	for(auto id : spatialIndexer->query(drawArea)) {
    		auto& mc = componentManager->moveComponents.at(id);
    		auto& rc = componentManager->renderComponents.at(id);

    		//Get the intersection between an entity within drawarea and the drawarea
    		const auto intersection = Rect::getIntersection(
                drawArea, spatialIndexer->getBoundingBox(id)
            );

    		//Only draw that portion of the texture that intersects
    		calculateZIndex(id);

            const auto clipSource = intersectionToClipSource(
                intersection,
                mc.pos,
                rc.xoffset, rc.yoffset
            );

            const auto clipDestination = intersectionToClipDestination(
                intersection
            );

            const RenderData renderData = {
                id, &rc, rc.textureData, clipSource, clipDestination
            };

    		pq.insert(renderData);
    	}

        //Dont move this piece of code. A reference to this element is used above!
        drawQueue.pop();
    }

    //Draw to worldTexture
	SDL_SetRenderTarget(renderer, worldTexture);

	while(!pq.isEmpty()) {
		render(pq.poll());
	}

    //Put the fps on tmp (text->surface->tmp->fontTexture->default render target)
    const std::string str = std::to_string((int)(1/deltaTime->delta())) + "fps";
    printText(Text(str, 0, 0, {231, 195, 175}));

    //If all active ids werent rendered this frame, warn
    if(allowedRendersThisFrame <= 0) {
        const std::string str = "WARNING: renderer is lagging behind";
        printText(Text(str, 0, 20, {255, 55, 55}));
    }

    //Render all texts
    renderTexts();

    //Draw to default render target (NULL)
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, worldTexture, &cameraSystem->getCamera(), NULL);
    SDL_RenderCopy(renderer, fontTexture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

unsigned int RenderSystem::count() const {
	return ids.size();
}

void RenderSystem::render(const RenderData& rd) const {
    SDL_RenderCopy(renderer, rd.textureData.texture, &rd.cliprect, &rd.target);
}

const string RenderSystem::getIdentifier() const {
	return "RenderSystem";
}

void RenderSystem::calculateZIndex(ID id) {
	auto& rc = componentManager->renderComponents.at(id);
	auto& mc = componentManager->moveComponents.at(id);
	rc.zindex = mc.pos.y + rc.textureData.height + rc.yoffset;
}

void RenderSystem::activateId(ID id) {
	//Only make the id active if it is a member of rendersystem
	if(ids.find(id) != ids.end()) {
		activeIds.push(id);
	}
}

void RenderSystem::setCameraSystem(CameraSystem* cameraSystem) {
	this->cameraSystem = cameraSystem;
}

void RenderSystem::setImage(ID id, string path) {
	auto& rc = componentManager->renderComponents.at(id);
	rc.imagePath = path;
	rc.textureData = textureDatas.at(path);
    activateId(id);
}

void RenderSystem::printText(const Text& text) {
    texts.push(text);
}

void RenderSystem::renderTexts() {
    //Deal with text. Pops texts from the 'texts'-queue
    //printing the texts using data from texts-class
    SDL_SetRenderTarget(renderer, fontTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    while(!texts.empty()) {
        const Text text = texts.front(); texts.pop();
        //Put the fps on tmp (text->surface->tmp->fontTexture->default render target)
        auto fontSurface = TTF_RenderText_Solid(font, text.text.c_str(), text.color);
        auto tmpTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
        int w, h;
        TTF_SizeText(font, text.text.c_str(), &w, &h);
        const SDL_Rect tmpdest = {text.x, text.y, w, h};
        SDL_RenderCopy(renderer, tmpTexture, nullptr, &tmpdest);
        SDL_FreeSurface(fontSurface);
        SDL_DestroyTexture(tmpTexture);
    }
}
