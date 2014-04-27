#include <stdint.h>

uint32_t functionYO(uint32_t x, uint32_t y) {
	os_printf("%d\n", x + y);
	return x + y;
}

int main() {
	functionYO(1, 3);
	
}
