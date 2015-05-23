#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H

class MoveComponent {
public:
    struct Point {
        float x, y;
    };

    struct Vector2 {
        float x, y;
    };

    Point pos;
    Point oldPos;
    Vector2 vel;

	MoveComponent(
		const Point& = {0, 0},
		const Point& = {0, 0},
		const Vector2& = {100, 100});
	virtual ~MoveComponent();
	void virtual move();
};

#endif //MOVECOMPONENT_H
