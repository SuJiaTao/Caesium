// Bailey Jia-Tao Brown
// 2023
// <csmint_memory.c>

#include "csmint_memory.h"

PVOID CInternalAlloc(SIZE_T size) {
	_CSyncEnter();
	PVOID block = HeapAlloc(_csmint.heap, ZERO, size);
	ZERO_BYTES(block, size);
	_csmint.allocateCount++;
	_CSyncLeave(block);
}

void  CInternalFree(PVOID ptr) {
	_CSyncEnter();
	HeapFree(_csmint.heap, ZERO, ptr);
	_csmint.allocateCount--;
	_CSyncLeave();
}
