#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H

class TileComponent {
 public:
	enum TYPE {
		GRASS,
		SAND,
		DIRT,
		WATER,
	};

	TYPE type{GRASS};
};

#endif //TILECOMPONENT_H