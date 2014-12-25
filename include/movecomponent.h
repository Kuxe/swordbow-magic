#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H

class MoveComponent {
 public:
	float xpos;
	float ypos;

	float xspeed;
	float yspeed; 

	MoveComponent(
		const float& xpos = 0,
		const float& ypos = 0,
		const float& xspeed = 100,
		const float& yspeed = 100);
	virtual ~MoveComponent();

	void virtual move();
};

#endif //MOVECOMPONENT_H