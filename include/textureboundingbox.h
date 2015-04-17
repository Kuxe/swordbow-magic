#ifndef TEXTUREDBOUNDINGBOX_H
#define TEXTUREDBOUNDINGBOX_H

#include "boundingbox.h"

class TextureBoundingBox : public BoundingBox {
public:
    TextureBoundingBox(ComponentManager* componentManager);
    unsigned int getX(ID id);
    unsigned int getY(ID id);
    unsigned int getOldX(ID id);
    unsigned int getOldY(ID id);
    unsigned int getW(ID id);
    unsigned int getH(ID id);
};

#endif //TEXTUREDBOUNDINGBOX_H
