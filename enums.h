/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __ENUMS_H__
#define __ENUMS_H__

// Widget IDs for callbacks
enum GLUI_WIDGET_ID
{
    CHILD_NODE_LB_ID = 200,
    CHILD_NODE_SELECT_B_ID = 201,
    PARENT_NODE_SELECT_B_ID = 202,
    NODE_TYPE_LB_ID = 203,
    CHILD_NODE_ADD_B_ID = 204,
    PARENT_NODE_ADD_B_ID = 205
};


// Shading mode to consider when rendering Trimesh object
enum NORM_FLAG_ID
{
    NO_NORM = 0,    // Disbale normals
    FACE_NORM,      // Face normals
    VERT_NORM       // Vertex normals
};

// Coordinate system To be taken into account when applying transformation
enum TRANS_FLAG_ID
{
    VIEW_COORDS = -1,   // Viewer's local coordinate system
    WORLD_COORDS = 1    // World coordinate syste,
};

// Type of transformation
enum TRANS_ID
{
    INVALID = 0,
    SCALE,          // Scale
    TRANSLATE,      // Translate
    ROTATE          // Rotate
};

enum LIGHT_ID
{
    POINT_LIGHT,
    DIRECTION_LIGHT
};

// Primary commands
enum CLI_COMMAND_ID
{
    CLI_LOAD = 'L',         // Push object onto view stack
    CLI_DELETE = 'D',       // Pop object from view stack
    CLI_IDENTITY = 'I',     // Identity matrix (clear transforms)
    CLI_TRANSLATE = 'T',    // Translate
    CLI_SCALE = 'S',        // Scale
    CLI_ROTATE = 'R',       // Rotate
    CLI_VIEW = 'V',         // Use view coordinates
    CLI_WORLD = 'W',        // Use world coordinates
    CLI_MODE = 'M',         // Set Render mode
    CLI_DEBUG = 'Z',        // Debug toggle
    CLI_EXIT = 'Q'          // Quit
};

// Options for 'M' (render mode) command
enum MODE_ID
{
    MODE_POINTS = 'P',      // Points
    MODE_WIREFRAME = 'W',   // Wireframe
    MODE_SOLID = 'S',       // Solid
    MODE_SHADED = 'L',      // Shaded
    MODE_FACE_NORMS = 'F',  // Face normals
    MODE_VERT_NORMS = 'V'   // Vertex normals
};

#endif
