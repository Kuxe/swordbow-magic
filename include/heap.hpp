#ifndef HEAP_H
#define HEAP_H

#include "dynamicarray.hpp"
#include <iostream>

template<class T>
class heap {
	private:
		class Node {

		};

		dynamicarray<T> dynarr;
		unsigned int lastIndex = 0;

		inline void swap(unsigned int index1, unsigned int index2) {
			const T temp = dynarr[index1];
			dynarr[index1] = dynarr[index2];
			dynarr[index2] = temp;

		}

		unsigned int lowestChildIndex(unsigned int index) {
			if((index+1)*2-1 >= dynarr.getSize()) {
				return index;
			} else if((index+1)*2 >= dynarr.getSize()) {
				return (index+1)*2-1;
			} else {
				return dynarr[(index+1)*2-1] < dynarr[(index+1)*2] ? (index+1)*2-1 : (index+1)*2;
			}
		}

		void bubbleUp(unsigned int index) {
			if(dynarr[index/2] > dynarr[index]) {
				swap(index/2, index);
				bubbleUp(index/2);
			}
		}

		void bubbleDown(unsigned int index) {
			const auto low = lowestChildIndex(index);
			if(dynarr[index] > dynarr[low]) {
				swap(index, low);
				bubbleDown(low);
			}
		}

	public:

		heap(unsigned int initialCapacity = 4) :
			dynarr(initialCapacity) {

		}

		void insert(T elem) {
			lastIndex = dynarr.add(elem);
			bubbleUp(lastIndex);
		}

		/**
		 *	returns element with highest priority
		 *	precon: heap is not empty
		 **/
		T poll() {
			T returnVal = dynarr[0];
			dynarr[0] = dynarr[lastIndex];
			dynarr.remove(lastIndex);
			bubbleDown(0);
			lastIndex--;
			return returnVal;
		}

		inline bool isEmpty() { return dynarr.getSize() == 0 ? true : false; }
};

#endif //HEAP_H
