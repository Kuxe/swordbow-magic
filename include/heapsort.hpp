#ifndef HEAPSORT_H
#define HEAPSORT_H

template<class T>
void bubbleDown(T* arr, unsigned int index, unsigned int lastElement) {
	//Compare children
	auto right = (index + 1)*2;
	auto left = (index + 1)*2-1;

	while(true) {
		if(right > lastElement) {
			//If right is larger than lastElement, ie theres no right...
			//check if theres a left atleast
			if(left > lastElement) {
				//Even left wasnt a children, so do nothing
				return;
			}
			//Only has a left child. Compare with it and swap if neccesary
			if(arr[index] < arr[left]) {
				const T temp = arr[left];
				arr[left] = arr[index];
				arr[index] = temp;
			}
			return;
		}
		//Determine which of left and right children are the greatest
		const auto greatest = arr[right] > arr[left] ? right : left;
		//If parent is lesser than the greatest, swap
		if(arr[index] < arr[greatest]) {
			const T temp = arr[greatest];
			arr[greatest] = arr[index];
			arr[index] = temp;

			//Continue bubbling down the, former, parent
			//could make recursive call here, but its better to
			//just index=greatest
			index = greatest;
			right = (index + 1)*2;
			left = (index + 1)*2-1;
		} else {
			//This is the spot for index, done
			return;
		}
	}
}

template<class T>
void heapsort(T* arr, unsigned int size) {

	//int arr[3], lastIndex is 2 (size-1)
	auto lastElement = size-1;

	//Initialization phase in the algorithm
	//make sure the array is ordered as a reversed POV-tree
	for(auto parent = (lastElement+1)/2; parent != 0; parent--) {
		bubbleDown(arr, parent-1, lastElement);
	}

	//Enter phase two!
	//At this point, the array is structured as a POV-tree
	//The largest element is at the root, the second and third largest elements
	//are children to the root so on so forth.
	//Swap root with the last element. Bubble the new root (former last element)
	//to its position. The second new root is the largest element. Swap it down.
	//rince repeat

	while(lastElement > 0) {
		//swap root and last element
		const T temp = arr[0];
		arr[0] = arr[lastElement];
		arr[lastElement] = temp;
		//bubble down the root
		bubbleDown(arr, 0, --lastElement);
	}
}

#endif //HEAPSORT_H
