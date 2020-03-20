//LoadTLE.h 
#ifndef _LOAD_TLE_H
#define _LOAD_TLE_H

#include <TLEType.h>
#include <SGP.h>

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>
#include <chrono>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265359
#endif //M_PI

bool LoadTLE(std::vector<sgp4_t> &sgpVector, std::string filename);

#endif //_LOAD_TLE_H
