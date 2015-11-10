/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __CAMERA_NODE_H__
#define __CAMERA_NODE_H__

#include "node.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class CameraNode : public Node
{
private:
    glm::vec3 camera_pos;
    glm::vec3 look_at_pos;
    glm::vec3 forward_vec;
    glm::vec3 right_vec;
    glm::vec3 up_vec;

    // Orbit
    bool orbit_flag;
    int start_orbit_x;
    int start_orbit_y;
    GLfloat orbit_theta;
    GLfloat orbit_phi;

    // Zoom
    bool zoom_flag;
    float start_zoom_y;
    GLfloat orbit_radius;

    // Pan
    bool pan_flag;
    int start_pan_x;
    int start_pan_y;

    // Field-of-View
    float fov;

    // Viewport
    int vx;
    int vy;

    void updateVectors(void);
    void updateCameraGivenOrbit(void);
    void updateCameraGivenPan(GLfloat x_off = 0.0, GLfloat y_off = 0.0);

public:
    CameraNode(void);
    CameraNode(Node * parent);
    void updateCameraGivenParams(glm::vec3 look_at_pos = glm::vec3(0.0, 0.0, 0.0), GLfloat orbit_radius = 10.0, GLfloat orbit_theta = -90.0, GLfloat orbit_phi = 0.0);
    float getFOV(void);
    int getViewportX(void);
    int getViewportY(void);
    void setFOV(float fov = 45.0);
    void setViewportXY(int vx = 0, int vy = 0);
    void processMouseButton(int button, int state, int x, int y);
    void processMouseMotion(int x, int y);
    void traverseNode(glm::mat4 transform = glm::mat4(1.0), std::string render_type = "Solid");
};

#endif