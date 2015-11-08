#include "geom.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

/// Vertex
// Set the vertex's position
void vertex_t::setPos(float pos[])
{
    this->pos.x = pos[0];
    this->pos.y = pos[1];
    this->pos.z = pos[2];
}

// Add a triangle id to the triangle adjacency list (used for normal calculation)
void vertex_t::addTriId(int tri_id)
{
    tri_ids.push_back(tri_id);
}

// Assignment operator overload (needed for pushing Trimesh onto stack)
vertex_t& vertex_t::operator=(const vertex_t& src)
{
    pos = src.pos;
    norm = src.norm;
    tri_ids = src.tri_ids;

    return *this;
}

/// Triangle
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
        vert_ids[i] = src.vert_ids[i];

    center = src.center;
    norm = src.norm;

    return *this;
}

/// Trimesh
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
        glm::vec3 u = _vertices[_triangles[i].vert_ids[1]].pos - _vertices[_triangles[i].vert_ids[0]].pos;

        // Compute the vector V defined as C - B
        glm::vec3 v = _vertices[_triangles[i].vert_ids[2]].pos - _vertices[_triangles[i].vert_ids[1]].pos;
        
        // ... And the normal of the triangle is defined as U x V
        _triangles[i].norm = glm::cross(u, v);
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
        glm::vec3 total = glm::vec3(0.0, 0.0, 0.0);
        int tri_ids_vec_size = _vertices[i].tri_ids.size();
        
        for (int j = 0; j < tri_ids_vec_size; j++)
            total = total + _triangles[_vertices[i].tri_ids[j]].norm;

        // ... And the normal of the vertex is defined as the average of this sum
        _vertices[i].norm = (GLfloat)(1.0 / tri_ids_vec_size) * total;
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
    _center.x = (_bounds[0][0] + _bounds[0][1]) / 2;
    _center.y = (_bounds[1][0] + _bounds[1][1]) / 2;
    _center.z = (_bounds[2][0] + _bounds[2][1]) / 2;
}

// Get the side length of a bounding cube for the object
GLfloat Trimesh::getBoundingLength(void)
{
    GLfloat max = _bounds[0][1] - _bounds[0][0];

    for (int i = 1; i < 3; i++)
    {
        max = ((_bounds[i][1] - _bounds[i][0]) > max ? _bounds[i][1] - _bounds[i][0] : max);
    }

    return max;
}

// Get center of object (usually for use with camera)
glm::vec3 Trimesh::getCenter(void)
{
    _calculateCenter();

    return _center;
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
    GLfloat tri_bounds[3][2] = { 
        { _vertices[vert_ids[0]].pos.x, _vertices[vert_ids[0]].pos.x },
        { _vertices[vert_ids[0]].pos.y, _vertices[vert_ids[0]].pos.y },
        { _vertices[vert_ids[0]].pos.z, _vertices[vert_ids[0]].pos.z }
    };

    for (int i = 1; i < 3; i++)
    {
        tri_bounds[0][0] = (_vertices[vert_ids[i]].pos.x < tri_bounds[0][0] ? _vertices[vert_ids[i]].pos.x : tri_bounds[0][0]);
        tri_bounds[0][1] = (_vertices[vert_ids[i]].pos.x > tri_bounds[0][1] ? _vertices[vert_ids[i]].pos.x : tri_bounds[0][1]);

        tri_bounds[1][0] = (_vertices[vert_ids[i]].pos.y < tri_bounds[1][0] ? _vertices[vert_ids[i]].pos.y : tri_bounds[1][0]);
        tri_bounds[1][1] = (_vertices[vert_ids[i]].pos.y > tri_bounds[1][1] ? _vertices[vert_ids[i]].pos.y : tri_bounds[1][1]);

        tri_bounds[2][0] = (_vertices[vert_ids[i]].pos.z < tri_bounds[2][0] ? _vertices[vert_ids[i]].pos.z : tri_bounds[2][0]);
        tri_bounds[2][1] = (_vertices[vert_ids[i]].pos.z > tri_bounds[2][1] ? _vertices[vert_ids[i]].pos.z : tri_bounds[2][1]);
    }

    // Compute and set midpoint of triangle
    tri.center.x = (tri_bounds[0][0] + tri_bounds[0][1]) / 2;
    tri.center.y = (tri_bounds[1][0] + tri_bounds[1][1]) / 2;
    tri.center.z = (tri_bounds[2][0] + tri_bounds[2][1]) / 2;

    _triangles.push_back(tri);
}

// Use OpenGL to render the object and possibly render face or vertex normals
void Trimesh::render(std::string render_type)
{
    // First calculate all necessary information for the object (center, normals, etc.)
    _constructObject();

    glMatrixMode(GL_MODELVIEW);

    bool vertex_normals = (render_type == "Vertex Normals");
    bool face_normals = (render_type == "Face Normals");
    int tri_vec_size = _triangles.size();

    // Loop through and draw each triangle
    glBegin(GL_TRIANGLES);

        // Set all vertex colors to white
        glColor4f(1.0, 1.0, 1.0, 1.0);

        // Set all normals to default normal
        glNormal3f(0.0, 0.0, 1.0);

        for (int i = 0; i < tri_vec_size; i++)
        {
            // Set normal for triangle if specified by show_norm_flag
            if (face_normals)
                glNormal3fv(glm::value_ptr(_triangles[i].norm));

            for (int j = 0; j < 3; j++)
            {
                // Set normal for vertex if specified by show_norm_flag
                if (vertex_normals)
                    glNormal3fv(glm::value_ptr(_vertices[_triangles[i].vert_ids[j]].norm));

                // Submit vertex position
                glVertex3fv(glm::value_ptr(_vertices[_triangles[i].vert_ids[j]].pos));
            }

        }

    glEnd();

    if (face_normals || vertex_normals)
    {
        // We want the vector lines to be approximately 1.5% the length of the bounding cube
        GLfloat line_length = getBoundingLength() * 0.015;

        glBegin(GL_LINES);

            // Color normal vectors red
            glColor4f(1.0, 0.0, 0.0, 1.0);

            // Set all vertex normals to default normal
            glNormal3f(0.0, 0.0, 1.0);

            if (face_normals)
            {
                // Show face normal vectors at triangle midpoints
                for (int i = 0; i < tri_vec_size; i++)
                {
                    glm::vec3 norm_line_end = _triangles[i].center + (line_length * glm::normalize(_triangles[i].norm));

                    // Submit line start and end positions
                    glVertex3fv(glm::value_ptr(_triangles[i].center));
                    glVertex3fv(glm::value_ptr(norm_line_end));
                }
            }
            else
            {
                // Show vertex normal vectors at vertex position
                for (int i = 0; i < tri_vec_size; i++)
                {
                    for (int j = 0; j < 3; j++)
                    {
                        glm::vec3 norm_line_end = _vertices[_triangles[i].vert_ids[j]].pos + (line_length * glm::normalize(_vertices[_triangles[i].vert_ids[j]].norm));

                        // Submit line start and end positions
                        glVertex3fv(glm::value_ptr(_vertices[_triangles[i].vert_ids[j]].pos));
                        glVertex3fv(glm::value_ptr(norm_line_end));
                    }
                }
            }

        glEnd();
    }
}

// Assignment operator overload (needed for pushing onto stack)
Trimesh& Trimesh::operator=(const Trimesh& src)
{
    _changed = src._changed;
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