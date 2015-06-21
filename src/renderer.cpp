#include "renderer.hpp"
#include "heap.hpp"

Renderer::Renderer() {
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

            "./resources/images/stone1.png",

            "./resources/images/water.png",

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

Renderer::~Renderer() {
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

void Renderer::render(heap<RenderData>& pq, const SDL_Rect& camera) {
    SDL_SetRenderTarget(renderer, worldTexture);

    while(!pq.isEmpty()) {
        const auto renderData = pq.poll();
        SDL_RenderCopy(
            renderer,
            textureDatas.at(renderData.texturePath).texture,
            &renderData.cliprect,
            &renderData.target
        );
	};

    //Render all texts
    renderTexts();

    //Draw to default render target (NULL)
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, worldTexture, &camera, NULL);
    SDL_RenderCopy(renderer, fontTexture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void Renderer::printText(const Text& text) {
    texts.push(text);
}

void Renderer::renderTexts() {
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

const unordered_map<string, TextureData>& Renderer::getTextureDatas() const {
    return textureDatas;
}


























//
