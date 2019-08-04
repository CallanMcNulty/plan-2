#include <math.h>
#include "globals.h"

bool approx(float a, float b) {
	return abs(a - b) < 0.0001;
}

bool approx_lt(float a, float b) {
	return approx(a, b) || a < b;
}

bool approx_gt(float a, float b) {
	return approx(a, b) || a > b;
}