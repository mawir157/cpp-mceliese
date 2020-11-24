#pragma once

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
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
