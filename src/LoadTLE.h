//LoadTLE.h 
#ifndef _LOAD_TLE_H
#define _LOAD_TLE_H

#include <TLEType.h>

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif //M_PI

bool LoadTLE(std::vector<tle_t> &tleVector, std::string filename);

#endif //_LOAD_TLE_H
