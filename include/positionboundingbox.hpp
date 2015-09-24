#ifndef POSITIONBOUNDINGBOX_HPP
#define POSITIONBOUNDINGBOX_HPP

#include "boundingbox.hpp"

class PositionBoundingBox : public BoundingBox {
private:
    
public:
    PositionBoundingBox(
        const ComponentManager* const componentManager
    );
    const Rect getBoundingBox(ID id) const;
    const Rect getOldBoundingBox(ID id) const;
    const System::Identifier getSystemName() const;
};

#endif //POSITIONBOUNDINGBOX_HPP
