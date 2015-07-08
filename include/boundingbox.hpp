#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "spatialindexer.hpp"
#include "systemidentifiers.hpp"

class ComponentManager;

typedef unsigned int ID;
class BoundingBox {
protected:
        const ComponentManager* const componentManager;
public:

    BoundingBox(const ComponentManager* const componentManager) :
        componentManager(componentManager) {}
    virtual const Rect getBoundingBox(ID id) const = 0;
    virtual const Rect getOldBoundingBox(ID id) const = 0;
    virtual const System getSystemName() const = 0;
};





#endif //BOUNDINGBOX_H
