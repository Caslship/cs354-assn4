/*
* Assignment #3
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __ENUMS_H__
#define __ENUMS_H__

enum GLUI_WIDGET_ID
{
/********** User IDs for callbacks ********/
    LIGHT0_ENABLED_ID = 200,
    LIGHT1_ENABLED_ID = 201,
    LIGHT0_INTENSITY_ID = 250,
    LIGHT1_INTENSITY_ID = 260,
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
    SCALE = 1,      // Scale
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
