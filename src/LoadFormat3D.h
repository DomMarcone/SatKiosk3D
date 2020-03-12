//LoadFormat3D.h

#ifndef _LOAD_FORMAT_3D_H
#define _LOAD_FORMAT_3D_H

#include <Format3D.h>
#include <LoadText.h>
#include <LoadBin.h>
#include <SaveBin.h>
#include <BinToFormat3D.h>
#include <Format3DToBin.h>
#include <PlyToFormat3D.h>

#include <iostream>
#include <string>

Format3D *LoadFormat3D(std::string filename);

#endif //_LOAD_FORMAT_3D_H
