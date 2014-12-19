#ifndef MOVECOMPONENT_H
#define MOVECOMPONENT_H

class MoveComponent {
 public:
	float xpos = 0;
	float ypos = 0;

	float xspeed = 0;
	float yspeed = 0; 

	MoveComponent(
		const float& xpos = 0,
		const float& ypos = 0,
		const float& xspeed = 0,
		const float& yspeed = 0);
	virtual ~MoveComponent();

	void virtual move();
};

#endif //MOVECOMPONENT_H