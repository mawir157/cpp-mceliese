#pragma once

#include <algorithm>
#include <bitset>
#include <chrono>
// #include <cmath>
// #include <complex>
#include <cstdint>
#include <cstring>
// #include <ctime>
#include <fstream>
#include <iostream>
#include <map>
// #include <memory>
// #include <random>
// #include <set>
#include <string>
#include <tuple>
#include <vector>

#include <math.h>

enum RunMode { orthogonal,
	             generate_keys,
	             encode_message,
	             decode_message,
	             test_mode,
	             unknown };
