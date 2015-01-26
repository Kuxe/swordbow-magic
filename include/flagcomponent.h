#ifndef FLAGCOMPONENT_H
#define FLAGCOMPONENT_H

#define FLAG static constexpr unsigned char

//Component used by misc systems and for misc purposes
//Dirty but gets the work done
class FlagComponent {
 public:
	FLAG HAS_CHANGED = 0x01;
	FLAG CLEAR = 0x00;
	unsigned char flags = 0x00;
};


#endif //FLAGSCOMPONENT_H