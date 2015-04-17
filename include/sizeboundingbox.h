#ifndef SIZEBOUNDINGBOX_H
#define SIZEBOUNDINGBOX_H

#include "boundingbox.h"

class SizeBoundingBox : public BoundingBox {
public:
    SizeBoundingBox(ComponentManager* componentManager);
    unsigned int getX(ID id);
    unsigned int getY(ID id);
    unsigned int getOldX(ID id);
    unsigned int getOldY(ID id);
    unsigned int getW(ID id);
    unsigned int getH(ID id);
    const string getSystemName() const;
};

#endif //SIZEBOUNDINGBOX_H
