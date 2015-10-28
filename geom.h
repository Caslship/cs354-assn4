/*
* Assignment #3
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __GEOM_H__
#define __GEOM_H__

#include <GL/glut.h>
#include <list>
#include <iostream>
#include <vector>
#include <cmath>
#include "enums.h"
#include "debug.h"

/// Transformation
struct transformation_t
{
    TRANS_ID type;

    float xyz[3];
    float angle;

    void setParams(TRANS_ID, float[], float);
    transformation_t& operator=(const transformation_t&);
};

// Set the details of the transformation
void transformation_t::setParams(TRANS_ID type, float xyz[], float angle = 0.0)
{
    this->type = type;

    for (int i = 0; i < 3; i++)
        this->xyz[i] = xyz[i];

    this->angle = angle;

}

// Assignment operator overload (needed for pushing Trimesh onto stack)
transformation_t& transformation_t::operator=(const transformation_t& src)
{
    type = src.type;

    for (int i = 0; i < 3; i++)
        xyz[i] = src.xyz[i];

    angle = src.angle;

    return *this;
}

/// Vertex
struct vertex_t
{
    float pos[3];
    float norm[3];
    std::vector<int> tri_ids;

    void setPos(float[]);
    void addTriId(int);
    vertex_t& operator=(const vertex_t&);
};

// Set the vertex's position
void vertex_t::setPos(float pos[])
{
    for (int i = 0; i < 3; i++)
        this->pos[i] = pos[i];
}

// Add a triangle id to the triangle adjacency list (used for normal calculation)
void vertex_t::addTriId(int tri_id)
{
    tri_ids.push_back(tri_id);
}

// Assignment operator overload (needed for pushing Trimesh onto stack)
vertex_t& vertex_t::operator=(const vertex_t& src)
{
    for (int i = 0; i < 3; i++)
    {
        pos[i] = src.pos[i];
        norm[i] = src.norm[i];
    }

    tri_ids = src.tri_ids;

    return *this;
}

/// Triangle
struct triangle_t
{
    int vert_ids[3];
    float center[3];
    float norm[3];

    void setVertIds(int[]);
    triangle_t& operator=(const triangle_t&);
};

// Set ids of vertices to be used for this triangle
void triangle_t::setVertIds(int vert_ids[])
{
    for (int i = 0; i < 3; i++)
        this->vert_ids[i] = vert_ids[i];
}

// Assignment operator overload (needed for pushing Trimesh onto stack)
triangle_t& triangle_t::operator=(const triangle_t& src)
{
    for (int i = 0; i < 3; i++)
    {
        vert_ids[i] = src.vert_ids[i];
        center[i] = src.center[i];
        norm[i] = src.norm[i];
    }

    return *this;
}

/// Trimesh
class Trimesh
{
private:
    bool _changed;
    std::list<transformation_t> _transformations;
    std::vector<vertex_t> _vertices;
    std::vector<triangle_t> _triangles;
    float _bounds[3][2];
    vertex_t _center;

    void _constructObject(void);
    void _calculateFaceNormals(void);
    void _calculateVertNormals(void);
    void _calculateNormals(void);
    void _calculateCenter(void);

public:
    Trimesh(void);
    float getBoundingLength(void);
    vertex_t getCenter(void);
    void clearTransformations(void);
    void addTransformation(TRANS_ID, float[], float);
    void addVertex(float[]);
    void addFace(int[]);
    void render(NORM_FLAG_ID, GLenum);
    Trimesh& operator=(const Trimesh&);
};

// Constructor just makes sure that we have not cached our object
Trimesh::Trimesh(void) : _changed(true) {}

// Construct all information necessary for rendering the object
void Trimesh::_constructObject(void)
{
    // If our cached object is dirty reconstruct everything
    if (_changed)
    {
        _calculateNormals();
        _calculateCenter();

        _changed = false;
    }
}

// Compute normals for each face
void Trimesh::_calculateFaceNormals(void)
{
    int tri_vec_size = _triangles.size();

    // Loop through each triangle
    for (int i = 0; i < tri_vec_size; i++)
    {
        // For vertices A, B, and C of a triangle (CCW order)...

        // Compute the vector U defined as B - A
        float u[3];
        for (int j = 0; j < 3; j++)
            u[j] = _vertices[_triangles[i].vert_ids[1]].pos[j] - _vertices[_triangles[i].vert_ids[0]].pos[j];

        // Compute the vector V defined as C - B
        float v[3];
        for (int j = 0; j < 3; j++)
            v[j] = _vertices[_triangles[i].vert_ids[2]].pos[j] - _vertices[_triangles[i].vert_ids[1]].pos[j];
        
        // ... And the normal of the triangle is defined as U x V
        _triangles[i].norm[0] = (u[1] * v[2]) - (u[2] * v[1]);
        _triangles[i].norm[1] = (u[2] * v[0]) - (u[0] * v[2]);
        _triangles[i].norm[2] = (u[0] * v[1]) - (u[1] * v[0]);
    }
}

// Compute normals for each vertex (must have called computed surface normals first)
void Trimesh::_calculateVertNormals(void)
{
    int vert_vec_size = _vertices.size();

    // Loop through each vertex
    for (int i = 0; i < vert_vec_size; i++)
    {
        // Sum up the normals of the triangles adjacent to the vertex...
        float total[3] = {};
        int tri_ids_vec_size = _vertices[i].tri_ids.size();
        
        for (int j = 0; j < tri_ids_vec_size; j++)
        {
            for (int k = 0; k < 3; k++)
                total[k] += _triangles[_vertices[i].tri_ids[j]].norm[k];
        }

        // ... And the normal of the vertex is defined as the average of this sum
        for (int j = 0; j < 3; j++)
            _vertices[i].norm[j] = total[j] / tri_ids_vec_size;
    }
}

// Compute both surface and vertex normals
void Trimesh::_calculateNormals(void)
{
    // I feel nasty doing this, but I'm pretty sure order is essential here
    _calculateFaceNormals();
    _calculateVertNormals();
}

// Compute center of the object
void Trimesh::_calculateCenter(void)
{
    // Define center position as the midway point between low and high bounds
    float pos[3];
    for (int i = 0; i < 3; i++)
        pos[i] = (_bounds[i][0] + _bounds[i][1]) / 2;

    _center.setPos(pos);
}

// Get the side length of a bounding cube for the object
float Trimesh::getBoundingLength(void)
{
    float max = _bounds[0][1] - _bounds[0][0];

    for (int i = 1; i < 3; i++)
    {
        max = ((_bounds[i][1] - _bounds[i][0]) > max ? _bounds[i][1] - _bounds[i][0] : max);
    }

    return max;
}

// Get center of object (usually for use with camera)
vertex_t Trimesh::getCenter(void)
{
    _calculateCenter();

    return _center;
}

// In response to the 'I' command, clear the model transforms for this object
void Trimesh::clearTransformations(void)
{
    _transformations.clear();
}

// Add a transformation to the object for use in rendering
void Trimesh::addTransformation(TRANS_ID type, float xyz[], float angle = 0.0)
{
    transformation_t trans;
    trans.setParams(type, xyz, angle);
    _transformations.push_front(trans);
}

// Given a position in 3D space, add a vertex to the object
void Trimesh::addVertex(float pos[])
{
    // This lets us know that are cached object is dirty
    _changed = true;

    // Initialize model bounds if necessary
    if (!_vertices.size())
    {
        for (int i = 0; i < 3; i++)
        {
            _bounds[i][0] = pos[i];
            _bounds[i][1] = pos[i];
        }
    }

    // Set new model bounds if necessary
    for (int i = 0; i < 3; i++)
    {
        _bounds[i][0] = (pos[i] < _bounds[i][0] ? pos[i] : _bounds[i][0]);
        _bounds[i][1] = (pos[i] > _bounds[i][1] ? pos[i] : _bounds[i][1]);
    }

    vertex_t vert;
    vert.setPos(pos);
    _vertices.push_back(vert);
}

// Given vertex indices, add a triangle to the object
void Trimesh::addFace(int vert_ids[])
{
    // This lets us know that are cached object is dirty
    _changed = true;

    int tri_vec_size = _triangles.size();

    // Add triangle index to each vertices adjacency list
    for (int i = 0; i < 3; i++)
    {
        _vertices[vert_ids[i]].addTriId(tri_vec_size);
    }

    triangle_t tri;
    tri.setVertIds(vert_ids);

    // Compute bounds of triangle
    float tri_bounds[3][2] = { 
        { _vertices[vert_ids[0]].pos[0], _vertices[vert_ids[0]].pos[0] },
        { _vertices[vert_ids[0]].pos[1], _vertices[vert_ids[0]].pos[1] },
        { _vertices[vert_ids[0]].pos[2], _vertices[vert_ids[0]].pos[2] }
    };

    for (int i = 1; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            tri_bounds[j][0] = (_vertices[vert_ids[i]].pos[j] < tri_bounds[j][0] ? _vertices[vert_ids[i]].pos[j] : tri_bounds[j][0]);
            tri_bounds[j][1] = (_vertices[vert_ids[i]].pos[j] > tri_bounds[j][1] ? _vertices[vert_ids[i]].pos[j] : tri_bounds[j][1]);
        }
    }

    // Compute and set midpoint of triangle
    for (int i = 0; i < 3; i++)
        tri.center[i] = (tri_bounds[i][0] + tri_bounds[i][1]) / 2;

    _triangles.push_back(tri);
}

// Use OpenGL to render the object and possibly render face or vertex normals
void Trimesh::render(NORM_FLAG_ID show_norm_flag = NO_NORM, GLenum render_mode = GL_FILL)
{
    // First calculate all necessary information for the object (center, normals, etc.)
    _constructObject();

    glMatrixMode(GL_MODELVIEW);

    // Draw extra features such as a bounding rectangle if in debug mode
    if (debug_flag)
    {
        // Draw the bounding rectangle in wireframe mode so that we can see through it
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBegin(GL_QUADS);

            // Color the bounding rectangle white
            glColor4f(1.0, 1.0, 1.0, 1.0);

            // Front
            glVertex3f(_bounds[0][0], _bounds[1][1], _bounds[2][1]);
            glVertex3f(_bounds[0][0], _bounds[1][0], _bounds[2][1]);
            glVertex3f(_bounds[0][1], _bounds[1][0], _bounds[2][1]);
            glVertex3f(_bounds[0][1], _bounds[1][1], _bounds[2][1]);

            // Back
            glVertex3f(_bounds[0][0], _bounds[1][1], _bounds[2][0]);
            glVertex3f(_bounds[0][0], _bounds[1][0], _bounds[2][0]);
            glVertex3f(_bounds[0][1], _bounds[1][0], _bounds[2][0]);
            glVertex3f(_bounds[0][1], _bounds[1][1], _bounds[2][0]);

            // Top
            glVertex3f(_bounds[0][0], _bounds[1][1], _bounds[2][0]);
            glVertex3f(_bounds[0][0], _bounds[1][1], _bounds[2][1]);
            glVertex3f(_bounds[0][1], _bounds[1][1], _bounds[2][1]);
            glVertex3f(_bounds[0][1], _bounds[1][1], _bounds[2][0]);          

            // Bottom
            glVertex3f(_bounds[0][0], _bounds[1][0], _bounds[2][0]);
            glVertex3f(_bounds[0][0], _bounds[1][0], _bounds[2][1]);
            glVertex3f(_bounds[0][1], _bounds[1][0], _bounds[2][1]);
            glVertex3f(_bounds[0][1], _bounds[1][0], _bounds[2][0]); 

        glEnd();

        // Make sure to set rendering mode back to its original state
        glPolygonMode(GL_FRONT_AND_BACK, render_mode);
    }

    // Apply all transformations before drawing (we want to act like the object starts off at the origin)
    glTranslatef(_center.pos[0], _center.pos[1], _center.pos[2]);
    for (std::list<transformation_t>::iterator it = _transformations.begin(); it != _transformations.end(); ++it)
    {
        switch(it->type)
        {
            case SCALE:
            {
                glScalef(it->xyz[0], it->xyz[1], it->xyz[2]);
                break;
            }
            case TRANSLATE:
            {
                glTranslatef(it->xyz[0], it->xyz[1], it->xyz[2]);
                break;
            }
            case ROTATE:
            {
                glRotatef(it->angle, it->xyz[0], it->xyz[1], it->xyz[2]);
                break;
            }
            default:
                break;
        }
    }
    glTranslatef(-_center.pos[0], -_center.pos[1], -_center.pos[2]);

    int tri_vec_size = _triangles.size();

    // Loop through and draw each triangle
    glBegin(GL_TRIANGLES);

        // Unless we don't have norms specified, set all vertex colors to white
        glColor4f(1.0, 1.0, 1.0, 1.0);

        for (int i = 0; i < tri_vec_size; i++)
        {
            // Set normal for triangle if specified by show_norm_flag
            if (show_norm_flag == FACE_NORM)
                glNormal3fv(_triangles[i].norm);

            for (int j = 0; j < 3; j++)
            {
                // Set normal for vertex if specified by show_norm_flag
                if (show_norm_flag == VERT_NORM)
                    glNormal3fv(_vertices[_triangles[i].vert_ids[j]].norm);
                else if ((show_norm_flag == NO_NORM) && debug_flag)
                {
                    if (j == 0)
                        glColor4f(1.0, 0.0, 0.0, 1.0);
                    if (j == 1)
                        glColor4f(0.0, 1.0, 0.0, 1.0);
                    if (j == 2)
                        glColor4f(0.0, 0.0, 1.0, 1.0);
                }

                // Submit vertex position
                glVertex3fv(_vertices[_triangles[i].vert_ids[j]].pos);
            }

        }

    glEnd();

    // Draw extra features such as normal vectors if in debug mode
    if (debug_flag)
    {
        if (show_norm_flag != NO_NORM)
        {
            // We want the vector lines to be approximately 1.5% the length of the bounding cube
            float line_length = getBoundingLength() * 0.015;

            glBegin(GL_LINES);

                // Color normal vectors red
                glColor4f(1.0, 0.0, 0.0, 1.0);

                if (show_norm_flag == FACE_NORM)
                {
                    // Show face normal vectors at triangle midpoints
                    for (int i = 0; i < tri_vec_size; i++)
                    {
                        // Compute vector magnitude to normalize it
                        float magnitude = 0;
                        for (int j = 0; j < 3; j++)
                            magnitude += (_triangles[i].norm[j] * _triangles[i].norm[j]);
                        magnitude = sqrt(magnitude);

                        float norm_line_end[3];
                        for (int j = 0; j < 3; j++)
                        {
                            // Normalize
                            norm_line_end[j] = (_triangles[i].norm[j] / magnitude);
                            // Extend to specified line length
                            norm_line_end[j] *= line_length;
                            // Add to triangle center to get line endpoint
                            norm_line_end[j] += _triangles[i].center[j];
                        }

                        // Submit line start and end positions
                        glVertex3fv(_triangles[i].center);
                        glVertex3fv(norm_line_end);
                    }
                }
                else
                {
                    // Show vertex normal vectors at vertex position
                    for (int i = 0; i < tri_vec_size; i++)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            // Compute vector magnitude to normalize it
                            float magnitude = 0;
                            for (int k = 0; k < 3; k++)
                                magnitude += (_vertices[_triangles[i].vert_ids[j]].norm[k] * _vertices[_triangles[i].vert_ids[j]].norm[k]);
                            magnitude = sqrt(magnitude);

                            float norm_line_end[3];
                            for (int k = 0; k < 3; k++)
                            {
                                // Normalize
                                norm_line_end[k] = (_vertices[_triangles[i].vert_ids[j]].norm[k] / magnitude);
                                // Extend to specified line length
                                norm_line_end[k] *= line_length;
                                // Add to vertex position to get line endpoint
                                norm_line_end[k] += _vertices[_triangles[i].vert_ids[j]].pos[k];
                            }

                            // Submit line start and end positions
                            glVertex3fv(_vertices[_triangles[i].vert_ids[j]].pos);
                            glVertex3fv(norm_line_end);
                        }
                    }
                }

            glEnd();
        }
    }
}

// Assignment operator overload (needed for pushing onto stack)
Trimesh& Trimesh::operator=(const Trimesh& src)
{
    _changed = src._changed;
    _transformations = src._transformations;
    _vertices = src._vertices;
    _triangles = src._triangles;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            _bounds[i][j] = src._bounds[i][j];
        }
    }

    _center = src._center;

    return *this;
}

#endif
