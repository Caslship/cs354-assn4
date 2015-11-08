/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "geom.h"

void InitializeWindow(int& argc, char ** argv);
void InitializeGraphics(void);
void RegisterCallbacks(void);
void InitializeGUI(void);
void UpdateGUI(int old_children_vec_size = 0);
void UpdateCameraGivenMesh(Trimesh model);

#endif
