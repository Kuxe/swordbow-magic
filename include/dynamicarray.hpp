#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

template<class T>
class dynamicarray {
	private:
		unsigned int capacity = 0;
		unsigned int size = 0;
		T* arr;

	public:

		class Iterator {
			private:
				unsigned int index;
				const T* const arr;
			public:
				Iterator(const T* const arr, unsigned int index) :
					arr(arr),
					index(index) { }
				
				const Iterator& operator ++() {
					index++;
					return *this;
				}

				const Iterator operator ++(int) {
					Iterator tmp(*this);
					operator++();
					return tmp;
				}

				inline bool operator != (Iterator & other) const {
					return index != other.index;
				}
				
				inline const T& operator* () const { return arr[index]; }



		};

		dynamicarray(unsigned int initialCapacity = 4) {
			capacity = initialCapacity;
			arr = new T[capacity];	
		}

		dynamicarray(const dynamicarray& other) :
			capacity(other.capacity),
			size(other.size) {
			arr = new T[capacity];
			for(int i = 0; i < size; i++) {
				arr[i] = other[i];	
			}	
		}

		~dynamicarray() {
			delete[] arr;
		}

		inline Iterator begin() const { return Iterator(arr, 0); }
		inline Iterator end() const { return Iterator(arr, size); }

		T& operator[] (unsigned int index) {
			return *(arr+index);	
		}	

		/**
		 *	returns index of added element
		 *	expands size of array if size exceeds capacity
		 **/ 
		unsigned int add(T elem) {
			if(size == capacity) {
				const unsigned int newCapacity = (capacity * 1.5) + (capacity % 2 != 0 ? 1 : 0);
				T* temp = new T[newCapacity];
				for(int i = 0; i < capacity; i++) {
					temp[i] = arr[i];
				}
				delete[] arr;
				capacity = newCapacity;
				arr = temp;
			}
			arr[size] = elem;
			size++;
			return size-1;
		}

		/**
		 *	returns index of item elem, -1 if not found
		 **/
		long int getIndex(T elem) {
			for(int i = 0; i < size; i++) {
				if(arr[i] == elem) return i;
			}
			return -1;
		}

		/**
		 *	returns true if it could find and remove element
		 *	otherwise false
		 *	will not change capacity or shrink size
		 *	thus dynamicarray will never shrink in size
		 *	once it gained size
		 **/
		bool remove(T elem) {
			return remove(getIndex(elem));		
		}

		bool remove(unsigned int index) {
			if(index == -1) return false;
			for(int i = index; i < size-1; i++) {
				arr[i] = arr[i+1];
			}
			size--;
			return true;
		}

		unsigned int getSize() const { return size; }
		unsigned int getCapacity() const { return capacity; }
};

#endif //DYNAMICARRAY_H
