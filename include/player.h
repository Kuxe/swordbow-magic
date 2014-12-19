#ifndef PLAYER_H
#define PLAYER_H

class MoveComponent;

class Player {
 public:
	MoveComponent* moveComponent;

	explicit Player(MoveComponent* moveComponent);
	~Player();
};

#endif //PLAYER_H