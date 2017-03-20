#ifndef EVENTS_HPP
#define EVENTS_HPP

struct KeyEvent {
	int key;
	bool press; //true = key was pressed, false = key was released
};

struct MouseEvent {
	float x, y;
};

struct QuitEvent { };


#endif //EVENTS_HPP