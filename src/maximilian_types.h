#pragma once

#ifdef ARDUINO
#include <limits>
#define USE_FLOAT
#undef min
#undef max
#undef PI
#endif

#ifdef USE_FLOAT
typedef float maxi_float_t;
#else
typedef double maxi_float_t;
#endif
