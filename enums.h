/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __ENUMS_H__
#define __ENUMS_H__

#define VIEWING_DISTANCE_MIN  0.1
#define ZOOM_SPEED 0.05
#define ORBIT_SPEED 0.1
#define PAN_SPEED 0.01

// Widget IDs for callbacks
enum GLUI_WIDGET_ID
{
    CHILD_NODE_SELECT_B_ID = 201,
    PARENT_NODE_SELECT_B_ID = 202,

    NODE_TYPE_LB_ID = 203,
    CHILD_NODE_ADD_B_ID = 204,
    PARENT_NODE_ADD_B_ID = 205,

    TRANSFORM_TYPE_LB_ID = 206,
    CURR_NODE_UPDATE_B_ID = 207,
    CURR_NODE_DELETE_B_ID = 208
};

#endif
