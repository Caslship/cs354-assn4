/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

void Display(void);
void Reshape(int w, int h);
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Idle(void);
void Control(int control_id);

#endif
