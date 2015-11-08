/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "cameranode.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "enums.h"

CameraNode::CameraNode(void) : camera_pos(0.0, 0.0, 10.0), look_at_pos(0.0, 0.0, 0.0), Node(NULL, "Camera")
{
    orbit_flag = false;
    zoom_flag = false;
    pan_flag = false;

    // Remember that the camera uses right handed coordinates
    orbit_theta = -90.0;
    orbit_phi = 0.0;
    orbit_radius = 10.0;

    vx = 0, vy = 0;

    updateCameraGivenOrbit();
}

CameraNode::CameraNode(Node * parent) : camera_pos(0.0, 0.0, 10.0), look_at_pos(0.0, 0.0, 0.0), Node(parent, "Camera")
{
    orbit_flag = false;
    zoom_flag = false;
    pan_flag = false;

    // Remember that the camera uses right handed coordinates
    orbit_theta = -90.0;
    orbit_phi = 0.0;
    orbit_radius = 10.0;

    vx = 0, vy = 0;

    updateCameraGivenOrbit();
}

void CameraNode::updateVectors(void)
{
    // Update front vector and then normalize it
    forward_vec.x = cos(glm::radians(orbit_theta)) * cos(glm::radians(orbit_phi));
    forward_vec.y = sin(glm::radians(orbit_phi));
    forward_vec.z = sin(glm::radians(orbit_theta)) * cos(glm::radians(orbit_phi));
    forward_vec = glm::normalize(forward_vec);

    // Update right vector by crossing with forward and world up vectors
    right_vec = glm::normalize(glm::cross(forward_vec, glm::vec3(0.0, 1.0, 0.0)));

    // Update up vector by crossing with right and forward vectors
    up_vec = glm::normalize(glm::cross(right_vec, forward_vec));  
}

void CameraNode::updateCameraGivenOrbit(void)
{
    updateVectors();

    camera_pos.x = look_at_pos.x - (orbit_radius * forward_vec.x);
    camera_pos.y = look_at_pos.y - (orbit_radius * forward_vec.y);
    camera_pos.z = look_at_pos.z - (orbit_radius * forward_vec.z);
}

void CameraNode::updateCameraGivenPan(GLfloat x_off, GLfloat y_off)
{
    glm::vec3 pos_off_vec = (x_off * right_vec) + (y_off * up_vec);

    look_at_pos += pos_off_vec;
    camera_pos += pos_off_vec;
}

void CameraNode::updateCameraGivenParams(glm::vec3 look_at_pos, GLfloat orbit_radius, GLfloat orbit_theta, GLfloat orbit_phi)
{
    this->look_at_pos = look_at_pos;
    this->orbit_radius = orbit_radius;
    this->orbit_theta = orbit_theta;
    this->orbit_phi = orbit_phi;

    updateCameraGivenOrbit();
}

int CameraNode::getViewportX(void)
{
    return vx;
}

int CameraNode::getViewportY(void)
{
    return vy;
}

void CameraNode::setViewportXY(int vx, int vy)
{
    this->vx = vx;
    this->vy = vy;
}

void CameraNode::processMouseButton(int button, int state, int x, int y)
{
    if ((orbit_flag = ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON))))
    {
        // Orbit
        start_orbit_x = x;
        start_orbit_y = y;
    }
    else if ((zoom_flag = ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON))))
    {
        // Zoom
        start_zoom_y = y;
    }
    else if ((pan_flag = ((state == GLUT_DOWN) && (button == GLUT_MIDDLE_BUTTON))))
    {
        // Pan
        start_pan_x = x;
        start_pan_y = y;
    }
}

void CameraNode::processMouseMotion(int x, int y)
{
    if (orbit_flag)
    {
        // Orbit
        orbit_theta += ((x - start_orbit_x) * ORBIT_SPEED);
        orbit_phi += ((start_orbit_y - y) * ORBIT_SPEED);

        if (orbit_phi > 89.0)
            orbit_phi = 89.0;
        else if (orbit_phi < -89.0)
            orbit_phi = -89.0;

        updateCameraGivenOrbit();

        start_orbit_x = x;
        start_orbit_y = y;
    }
    else if (zoom_flag)
    {
        // Zoom
        orbit_radius += ((y - start_zoom_y) * ZOOM_SPEED);
        if (orbit_radius < VIEWING_DISTANCE_MIN)
            orbit_radius = VIEWING_DISTANCE_MIN;

        updateCameraGivenOrbit();

        start_zoom_y = y;
    }
    else if (pan_flag)
    {
        // Pan
        updateCameraGivenPan((GLfloat)((start_pan_x - x) * PAN_SPEED), (GLfloat)((y - start_pan_y) * PAN_SPEED));

        start_pan_x = x;
        start_pan_y = y;
    }

    glutPostRedisplay();
}

void CameraNode::traverseNode(glm::mat4 transform, std::string render_type)
{
    glm::mat4 final_transform = glm::translate(glm::mat4(1.0), look_at_pos) * transform * glm::translate(glm::mat4(1.0), (GLfloat)-1.0 * look_at_pos);

    glm::vec4 final_camera_pos = final_transform * glm::vec4(camera_pos.x, camera_pos.y, camera_pos.z, 1.0);
    glm::vec4 final_look_at_pos = final_transform * glm::vec4(look_at_pos.x, look_at_pos.y, look_at_pos.z, 1.0);

    glm::mat4 view_mat = glm::lookAt(
        glm::vec3(final_camera_pos.x, final_camera_pos.y, final_camera_pos.z),
        glm::vec3(final_look_at_pos.x, final_look_at_pos.y, final_look_at_pos.z), 
        up_vec
        );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixf(glm::value_ptr(view_mat));
}