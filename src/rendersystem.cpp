#include "rendersystem.h"
#include <iostream>

using namespace std;

void RenderSystem::add(unsigned long long int* id, RenderData data) {
	renderDatas.insert(make_pair(id, data));
}

void RenderSystem::remove(unsigned long long int* id) {
	if(renderDatas.erase(id) == 0) {
		cout << "WARNING: RenderSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
}

void RenderSystem::update() {
	for(auto data : renderDatas) {
		render(get<1>(data));
	}
}

void RenderSystem::render(RenderData data) {
	//Dummy render-method
}

const string RenderSystem::getIdentifier() const {
	return "RenderSystem";
}