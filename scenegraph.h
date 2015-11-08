/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <GL/glut.h>
#include <stack>
#include "node.h"
#include "cameranode.h"

class SceneGraphContainer
{
private:
    Node * root_node;
    CameraNode * camera_node;
    Node * curr_node;
    int light_count;
    std::stack<GLenum> free_light_ids;

public:
    SceneGraphContainer(void);
    ~SceneGraphContainer(void);
    Node * getRootNode(void);
    CameraNode * getCameraNode(void);
    Node * getCurrentNode(void);;
    int getLightCount(void);
    bool removeLight(GLenum light_id = GL_LIGHT0);
    GLenum addLight(void);
    void setCurrentNode(Node * new_curr_node);
    void traverseGraph(void);
};

#endif