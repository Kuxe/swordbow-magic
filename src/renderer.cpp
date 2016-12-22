#include "renderer.hpp"

/** For logging **/
#include "logger.hpp"
#include <ostream>

/** For strmcp **/
#include <cstring>

Renderer::Renderer(bool fullscreen, bool vsync) {

    //Check if fullscreen was set from command-line
    Uint32 fullscreenFlag = 0;
    if(fullscreen) {
        Logger::log("Fullscreen on", Log::INFO);
        fullscreenFlag = SDL_WINDOW_FULLSCREEN;
    }

    Uint32 vsyncFlag = 0;
    if(vsync) {
        Logger::log("VSync on", Log::INFO);
        vsyncFlag = SDL_RENDERER_PRESENTVSYNC;
    }

    //Initialize SDL
    if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        std::ostringstream oss;
        oss << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        Logger::log(oss, Log::ERROR);
    } else {
        window = SDL_CreateWindow(
            "SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN | fullscreenFlag);

        if(!window) {
            std::ostringstream oss;
            oss << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            Logger::log(oss, Log::ERROR);
        } else {

            //Flags for renderer to be used
            Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | vsyncFlag;
            renderer = SDL_CreateRenderer(window, -1, rendererFlags);
            if(!renderer) {
                std::ostringstream oss;
                oss << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
                Logger::log(oss, Log::ERROR);
            } else {

				//imgFlags is used to initialize PNG-loading
				//so that IMG_Load can be used instead of SDL_LoadBMP
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
                    std::ostringstream oss;
					oss << "Couldnt initialze PNG usage!" << std::endl;
                    Logger::log(oss, Log::ERROR);
				} else {

		            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

                    fontTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
                    if(!fontTexture) {
                        std::ostringstream oss;
						oss << "Couldn't create fontTexture!" << std::endl;
						oss << "Screen will probably go black from now on" << std::endl;
						oss << "SDL_GetError(): " << SDL_GetError() << std::endl;
                        Logger::log(oss, Log::ERROR);
					}
                    SDL_SetTextureBlendMode(fontTexture, SDL_BLENDMODE_BLEND);

			        worldTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 8192, 8192);
					if(!worldTexture) {
                        std::ostringstream oss;
						oss << "Couldn't create worldTexture!" << std::endl;
						oss << "Screen will probably go black from now on" << std::endl;
						oss << "SDL_GetError(): " << SDL_GetError() << std::endl;
                        Logger::log(oss, Log::ERROR);
					}
					SDL_SetRenderTarget(renderer, worldTexture);
					SDL_RenderClear(renderer);

                    overlayTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
                    if(!overlayTexture) {
                        std::ostringstream oss;
                        oss << "Couldn't create overlayTexture!" << std::endl;
                        oss << "You won't see any loading screens or alike.. Screen could go all black" << std::endl;
                        oss << "SDL_GetError(): " << SDL_GetError() << std::endl;
                        Logger::log(oss, Log::ERROR);
                    }
                    SDL_SetTextureBlendMode(overlayTexture, SDL_BLENDMODE_BLEND);
				}

                if(TTF_Init() < 0 ) {
                    std::ostringstream oss;
                    oss << "Couldn't initialize SDL2_ttf!" << std::endl;
                    Logger::log(oss, Log::ERROR);
                } else {

                    const std::string fontPath("../resources/fonts/DejaVuSansMono.ttf");
                    font = TTF_OpenFont(fontPath.c_str(), 11);

                    if(!font) {
                        std::ostringstream oss;
                        oss << "Failed to load font!" << std::endl;
                        Logger::log(oss, Log::ERROR);
                    }
                }
            }
        }
    }

    /** Image paths are relative to the executable. If executable is here:
        /someFolder/<the executable is here>

        Then the resources are assumed to be in:
        /someFolder/resources/images/<image files here>

        If this isn't adhered to, images won't be found and segfaults will be inbound
    **/
    const std::string imageDirectory = "../resources/images/";
    const std::vector<std::pair<Image::Identifier, const std::string>> pairs {
        {Image::HELLO_WORLD, imageDirectory + "HelloWorld.bmp"},
        {Image::PLAYER, imageDirectory + "player.bmp"},
        {Image::GRASS, imageDirectory + "grass.png"},
        {Image::SMALL_TREE, imageDirectory + "SmallTree.png"},
        {Image::PLAYER_V2, imageDirectory + "playerv2.png"},


        {Image::PLAYER_V3_FRONT, imageDirectory + "playerv3_front.png"},
        {Image::PLAYER_V3_BACK, imageDirectory + "playerv3_back.png"},
        {Image::PLAYER_V3_LEFT, imageDirectory + "playerv3_left.png"},
        {Image::PLAYER_V3_RIGHT, imageDirectory + "playerv3_right.png"},

        {Image::PLAYER_V3_FRONT_RUN1, imageDirectory + "playerv3_front_run1.png"},
        {Image::PLAYER_V3_FRONT_RUN2, imageDirectory + "playerv3_front_run2.png"},
        {Image::PLAYER_V3_FRONT_RUN3, imageDirectory + "playerv3_front_run3.png"},
        {Image::PLAYER_V3_FRONT_RUN4, imageDirectory + "playerv3_front_run4.png"},

        {Image::BLOODSPLATTER_1_1, imageDirectory + "bloodsplatter1_1.png"},
        {Image::BLOODSPLATTER_1_2, imageDirectory + "bloodsplatter1_2.png"},
        {Image::BLOODSPLATTER_1_3, imageDirectory + "bloodsplatter1_3.png"},
        {Image::BLOODSPLATTER_1_4, imageDirectory + "bloodsplatter1_4.png"},
        {Image::BLOODSPLATTER_1_5, imageDirectory + "bloodsplatter1_5.png"},
        {Image::BLOODSPLATTER_1_6, imageDirectory + "bloodsplatter1_6.png"},
        {Image::BLOODSPLATTER_1_7, imageDirectory + "bloodsplatter1_7.png"},
        {Image::BLOODSPLATTER_1_8, imageDirectory + "bloodsplatter1_8.png"},
        {Image::BLOODSPLATTER_1_9, imageDirectory + "bloodsplatter1_9.png"},
        {Image::BLOODSPLATTER_1_10, imageDirectory + "bloodsplatter1_10.png"},
        {Image::BLOODSPLATTER_1_11, imageDirectory + "bloodsplatter1_11.png"},

        {Image::YELLOW_FLOWER, imageDirectory + "yellow_flower.png"},
        {Image::GREEN_FLOWER, imageDirectory + "green_flower.png"},
        {Image::VIOLET_FLOWER, imageDirectory + "violet_flower.png"},
        {Image::BLUE_FLOWER, imageDirectory + "blue_flower.png"},

        {Image::TEST_SQUARE_10x10, imageDirectory + "testsquare10x10.png"},
        {Image::TEST_SQUARE_1x1, imageDirectory + "testsquare1x1.png"},

        {Image::STONE_1, imageDirectory + "stone1.png"},

        {Image::WATER, imageDirectory + "water.png"},

        {Image::BLUE_BIRD_WEST_1, imageDirectory + "bluebird_west_1.png"},
        {Image::BLUE_BIRD_WEST_2, imageDirectory + "bluebird_west_2.png"},
        {Image::BLUE_BIRD_WEST_3, imageDirectory + "bluebird_west_3.png"},
        {Image::BLUE_BIRD_WEST_4, imageDirectory + "bluebird_west_4.png"},
        {Image::BLUE_BIRD_EAST_1, imageDirectory + "bluebird_east_1.png"},
        {Image::BLUE_BIRD_EAST_2, imageDirectory + "bluebird_east_2.png"},
        {Image::BLUE_BIRD_EAST_3, imageDirectory + "bluebird_east_3.png"},
        {Image::BLUE_BIRD_EAST_4, imageDirectory + "bluebird_east_4.png"},

        {Image::GREY_OVERLAY, imageDirectory + "overlay.png"},
        {Image::CONNECT_OVERLAY, imageDirectory + "overlay.png"},
        {Image::RECEIVING_DATA_OVERLAY, imageDirectory + "overlay.png"},
    };

    //Bind each texture with a value in Image and load the texture
    for(auto pair : pairs) {
        SDL_Surface* rawImage = IMG_Load(pair.second.c_str());
        if(!rawImage) {
            std::ostringstream oss;
            oss << "Couldn't load image on: " << pair.second << std::endl;
            Logger::log(oss, Log::ERROR);
        } else {
            //Make purple parts of images transparent by using color keying
            SDL_SetColorKey(rawImage, SDL_TRUE, SDL_MapRGB( rawImage->format, 0xFF, 0x00, 0xFF ));
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, rawImage);
            if(!texture) {
                std::ostringstream oss;
                oss << "Couldn't convert surface on addr " << rawImage << " to texture! SDL_Error: " << SDL_GetError() << std::endl;
                Logger::log(oss, Log::ERROR);
            } else {
                SDL_FreeSurface(rawImage);
                int w, h;
                SDL_QueryTexture(texture, NULL, NULL, &w, &h);
                textureDatas.insert({pair.first, {texture, {w, h}}});
            }
        }
    }
}

Renderer::~Renderer() {
    TTF_CloseFont(font);
    TTF_Quit();

    //Free all loaded images
    for(auto textureData : textureDatas) {
        SDL_DestroyTexture(std::get<1>(textureData).texture);
    }

    SDL_DestroyTexture(fontTexture);
    SDL_DestroyTexture(worldTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Renderer::render(std::priority_queue<RenderData>& pq, const SDL_Rect& camera) {

    SDL_SetRenderTarget(renderer, worldTexture);

    while(!pq.empty()) {
        const auto renderData = pq.top();
        SDL_RenderCopy(
            renderer,
            textureDatas.at(renderData.image).texture,
            &renderData.cliprect,
            &renderData.target
        );
        pq.pop();
	};

    //Render all texts
    renderTexts();

    //Ontop of everything, render overlays (if any)
    renderOverlays();

    //Draw to default render target (NULL)
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, worldTexture, &camera, NULL);
    SDL_RenderCopy(renderer, fontTexture, nullptr, nullptr);
    SDL_RenderCopy(renderer, overlayTexture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void Renderer::renderOnlyOverlays() {
    renderOverlays();
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, overlayTexture, nullptr, nullptr);
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

const std::unordered_map<Image::Identifier, TextureData, std::hash<int>>& Renderer::getTextureDatas() const {
    return textureDatas;
}

void Renderer::showOverlay(Image::Identifier identifier, Text text) {
    overlays.insert({identifier, Overlay(identifier, text)});
    auto& overlay = overlays.at(identifier);
    overlay.show();
}

void Renderer::hideOverlay(Image::Identifier identifier) {
    overlays.erase(identifier);
}

void Renderer::fadeInOverlay(Image::Identifier identifier, float seconds, Text text) {
    overlays.insert({identifier, Overlay(identifier, text)});
    auto& overlay = overlays.at(identifier);
    overlay.fadeIn(seconds);
}


void Renderer::fadeOutOverlay(Image::Identifier identifier, float seconds) {
    auto& overlay = overlays.at(identifier);
    overlay.fadeOut(seconds);
}

void Renderer::renderOverlays() {
    SDL_SetRenderTarget(renderer, overlayTexture);
    SDL_RenderClear(renderer);

    auto it = overlays.begin();
    for(it; it != overlays.end();) {
        auto& identifier = (*it).first;
        auto& overlay = (*it).second;
        auto& texture = textureDatas.at(identifier).texture;

        const float transparency = overlay.getTransparency();
        constexpr float eps = 0.00001f;
        if(transparency > 1.0f - eps) {
            it = overlays.erase(it);
        } else {
            it++;
            SDL_SetTextureAlphaMod(texture, (1 - transparency) * 255);
            auto& texture = textureDatas.at(overlay.identifier).texture;
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);

            const auto& text = overlay.getText();
            if(text.text != "") {
                auto fontSurface = TTF_RenderText_Solid(font, text.text.c_str(), text.color);
                auto tmpTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
                SDL_SetTextureAlphaMod(tmpTexture, (1 - transparency) * 255);
                int w, h;
                TTF_SizeText(font, text.text.c_str(), &w, &h);
                const SDL_Rect tmpdest = {text.x, text.y, w, h};
                SDL_RenderCopy(renderer, tmpTexture, nullptr, &tmpdest);
                SDL_FreeSurface(fontSurface);
                SDL_DestroyTexture(tmpTexture);
            }
        }
    }
}
























//
