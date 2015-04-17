#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

class ComponentManager;

typedef unsigned long long int* ID;
class BoundingBox {
protected:
        const ComponentManager* const componentManager;
public:

    BoundingBox(ComponentManager* componentManager) :
        componentManager(componentManager) {}
    virtual unsigned int getX(ID id) = 0;
    virtual unsigned int getY(ID id) = 0;
    virtual unsigned int getOldX(ID id) = 0;
    virtual unsigned int getOldY(ID id) = 0;
    virtual unsigned int getW(ID id) = 0;
    virtual unsigned int getH(ID id) = 0;
};





#endif //BOUNDINGBOX_H
