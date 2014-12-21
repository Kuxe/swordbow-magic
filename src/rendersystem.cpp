#include "rendersystem.h"
#include <iostream>

using namespace std;

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