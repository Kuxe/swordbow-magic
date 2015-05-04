#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <string>
#include "spatialindexer.hpp"

using std::string;

class ComponentManager;

typedef unsigned int ID;
class BoundingBox {
protected:
        const ComponentManager* const componentManager;
public:

    BoundingBox(const ComponentManager* const componentManager) :
        componentManager(componentManager) {}
    virtual const SpatialIndexer::Rect getBoundingBox(ID id) const = 0;
    virtual const SpatialIndexer::Rect getOldBoundingBox(ID id) const = 0;
    virtual const string getSystemName() const = 0;
};





#endif //BOUNDINGBOX_H
