#include "arrayfill.h"

void arrfill(int n) {
	int i;
	int temp = 1;
	for (i = 0; i < n; i++) {
		arr[i] = temp;
		temp++;
	}
}

int main () {
	arrfill(5);
	int i;
	int size = 5;
	for (i = 0; i < size; ++i) {
		printf("%d ", arr[i]);
	}
	printf("\n");
	return 0;
}