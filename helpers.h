/*
* Assignment #3
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <GL/glut.h>
#include <GL/glui.h>
#include <fstream>
#include <iostream>
#include <string>
#include "enums.h"
#include "callbacks.h"
#include "node.h"
#include "geom.h"
#include "loader.h"
#include "debug.h"

using namespace std;

// For some reason, having these as local objects cause segmentation faults
static TrimeshLoader model_loader;
static ifstream ifs;
static string prog_switch = "";
static string file_name = "";
static string x = "";
static string y = "";
static string z = "";
static string angle = "";

// Initalize GLUT and window properties
void InitializeWindow(int& argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    main_window = glutCreateWindow("Jason Palacios - Scene Graph Viewer");
}

// Initial settings for OpenGL
void InitializeGraphics(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
}

// Register all callbacks to be used with GLUT
void RegisterCallbacks(void)
{
    glutDisplayFunc(Display);
    GLUI_Master.set_glutReshapeFunc(Reshape);  
    GLUI_Master.set_glutKeyboardFunc(Keyboard);
    GLUI_Master.set_glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
}

string root_node_list[] = { "Camera", "Light", "Object" };
string node_type_list[] = { "Object", "Geometry", "Transform", "Attribute", "Light" };
string transform_type_list[] = { "Scale", "Translate", "Rotate" };

void InitializeGUI(void)
{
    glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);

    // Scene graph panel
    GLUI_Panel * scene_graph_panel = glui->add_panel("Edit Scene Graph");
    GLUI_Panel * select_node_panel = glui->add_panel_to_panel(scene_graph_panel, "Node Selection");
    GLUI_Listbox * node_select = glui->add_listbox_to_panel(select_node_panel, "Child ");
    for (int i = 0; i < 3; i++)
        node_select->add_item(i, root_node_list[i].c_str());
    glui->add_statictext_to_panel(select_node_panel, "");
    GLUI_Button * select_child_node = glui->add_button_to_panel(select_node_panel, "Select Child");
    select_child_node->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Button * select_parent_node = glui->add_button_to_panel(select_node_panel, "Select Parent");
    select_parent_node->set_alignment(GLUI_ALIGN_LEFT);
    select_node_panel->set_alignment(GLUI_ALIGN_LEFT);
    // glui->add_statictext_to_panel(scene_graph_panel, "");
    GLUI_Panel * add_node_panel = glui->add_panel_to_panel(scene_graph_panel, "Node Addition");
    GLUI_Listbox * node_type_select = glui->add_listbox_to_panel(add_node_panel, "Type ");
    for (int i = 0; i < 5; i++)
        node_type_select->add_item(i, node_type_list[i].c_str());
    glui->add_statictext_to_panel(add_node_panel, "");
    GLUI_Button * add_child_node = glui->add_button_to_panel(add_node_panel, "Add Child");
    add_child_node->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Button * add_parent_node = glui->add_button_to_panel(add_node_panel, "Add Parent");
    add_parent_node->set_alignment(GLUI_ALIGN_LEFT);
    add_node_panel->set_alignment(GLUI_ALIGN_LEFT);
    scene_graph_panel->set_alignment(GLUI_ALIGN_LEFT);

    // Current node panel
    GLUI_Panel * curr_node_panel = glui->add_panel("Edit Current Node");
    GLUI_Panel * attr_node_panel = glui->add_panel_to_panel(curr_node_panel, "Attribute");
    GLUI_Listbox * render_mode_select = glui->add_listbox_to_panel(attr_node_panel, "Render ", &curr_render);
    for(int i = 0; i < 6; i++ )
        render_mode_select->add_item(i, render_mode_list[i].c_str());
    attr_node_panel->set_alignment(GLUI_ALIGN_LEFT);
    // glui->add_statictext_to_panel(curr_node_panel, "");
    GLUI_Panel * geom_node_panel = glui->add_panel_to_panel(curr_node_panel, "Geometry");
    GLUI_EditText * geometry_path = glui->add_edittext_to_panel(geom_node_panel, "Obj");
    geom_node_panel->set_alignment(GLUI_ALIGN_LEFT);
    // glui->add_statictext_to_panel(curr_node_panel, "");
    GLUI_Panel * transform_node_panel = glui->add_panel_to_panel(curr_node_panel, "Transformation");
    GLUI_Listbox * transform_type_select = glui->add_listbox_to_panel(transform_node_panel, "Type ");
     for(int i = 0; i < 3; i++ )
        transform_type_select->add_item(i, transform_type_list[i].c_str());
    transform_type_select->set_alignment(GLUI_ALIGN_RIGHT);   
    GLUI_EditText * x_param = glui->add_edittext_to_panel(transform_node_panel, "X");
    GLUI_EditText * y_param = glui->add_edittext_to_panel(transform_node_panel, "Y");
    GLUI_EditText * z_param = glui->add_edittext_to_panel(transform_node_panel, "Z");
    GLUI_EditText * theta_param = glui->add_edittext_to_panel(transform_node_panel, "Theta");
    transform_node_panel->set_alignment(GLUI_ALIGN_LEFT);
    glui->add_statictext_to_panel(curr_node_panel, "");
    GLUI_Button * update_node = glui->add_button_to_panel(curr_node_panel, "Update");
    update_node->set_alignment(GLUI_ALIGN_LEFT);
    GLUI_Button * delete_node = glui->add_button_to_panel(curr_node_panel, "Delete");
    delete_node->set_alignment(GLUI_ALIGN_LEFT);
    curr_node_panel->set_alignment(GLUI_ALIGN_LEFT);


    /**** Link windows to GLUI, and register idle callback ******/

    glui->set_main_gfx_window(main_window);
    GLUI_Master.set_glutIdleFunc(Idle);
}

// Read command line arguments
void ExecuteArguments(int& argc, char ** argv)
{
    // Loop through arguments
    int i = 1;
    while ((argc - i) > 0)
    {
        prog_switch.assign(argv[i]);

        if (prog_switch.compare("-f") == 0)
        {
            // Load an input file

            i++;

            if ((argc - i) > 0)
            {
                cout << "Load input file: " << argv[i] << endl;

                // Open file
                ifs.open(argv[i]);

                if (!ifs.is_open() || !ifs.good() || ifs.fail())
                {
                    cout << "Invalid input file!" << endl;
                    cout.flush();
                    return;
                }

                // Execute each line as a command
                while (getline(ifs, command))
                {
                    if (!ExecuteCommand())
                    {
                        cout << "Invalid command or parameters!" << endl;
                        cout.flush();
                        return;
                    }

                    command.clear();
                }

                ifs.close();

                glutPostRedisplay();
            }
            else
            {
                cout << "Invalid input file!" << endl;
                cout.flush();
                return;
            }
        }
        else if (prog_switch.compare("-d") == 0)
        {
            // Run the program in debug mode

            debug_flag = true;
            cout << "Debug mode enabled" << endl;
        }
        else
        {
            cout << "Invalid switch!" << endl;
            cout.flush();
            return;
        }

        i++;
    }

    cout.flush();
}

// Executes a command and the return value indicates success or failure
bool ExecuteCommand(void)
{
    int pos = 0;
    int command_length = command.length();

    if (debug_flag)
        cout << "-- Command: " << command << endl;

    // Skip spaces but make sure not to hit the end of the string
    while (pos < command_length && command[pos] ==  ' ')
        pos++;

    if (pos == command_length)
        return false;

    // Read command type and update position
    char command_type = command[pos++];

    // Don't want anybody excluding spaces
    if (pos != command_length && command[pos] != ' ')
        return false;

    command_type = toupper(command_type);

    if (debug_flag)
        cout << "-- Type: " << command_type << endl;

    switch(command_type)
    {
        case CLI_LOAD:
        {
            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read file name and update position
            file_name = "";
            while (pos < command_length && command[pos] != ' ')
            {
                file_name += command[pos++];
            }

            // Append .obj extension to the string if needed
            int ext_pos = file_name.find(".obj");
            if (ext_pos == string::npos || ext_pos != file_name.length() - 4)
                file_name += ".obj";

            if (debug_flag)
                cout << "---- Load: " << file_name << endl;

            // Load object file into Trimesh object
            Trimesh model;

            // Fail if the object failed to load
            if (!model_loader.loadOBJ(file_name.c_str(), &model))
                return false;

            model_stack.push(model);

            // Adjust camera for new object
            look_at_pos = model.getCenter();
            zoom_level = model.getBoundingLength();
            zoom_level = (zoom_level < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : zoom_level);
            orbit_theta = orbit_phi = orbit_delta = pan_x = pan_y = 0.0;
            g_width = win_width;
            g_height = win_height;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

            glMatrixMode(GL_MODELVIEW);

            if (debug_flag)
            {
                cout << "---- Camera pos: { " << look_at_pos.pos[0] << ", " << look_at_pos.pos[1] << ", " << zoom_level << " }" << endl;
                cout << "---- Camera look at pos: { " << look_at_pos.pos[0] << ", " << look_at_pos.pos[1] << ", " << look_at_pos.pos[2] << " }" << endl;
            }

            break;
        }
        case CLI_DELETE:
        {
            if (!model_stack.empty())
            {
                if (debug_flag)
                    cout << "---- Delete: " << file_name << endl;

                // Pop off model from stack
                model_stack.pop();

                // Reset camera for old object
                if (!model_stack.empty())
                {
                    Trimesh model = model_stack.top();

                    look_at_pos = model.getCenter();
                    zoom_level = model.getBoundingLength();
                    zoom_level = (zoom_level < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : zoom_level);
                    orbit_theta = orbit_phi = orbit_delta = pan_x = pan_y = 0.0;
                    g_width = win_width;
                    g_height = win_height;

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

                    glMatrixMode(GL_MODELVIEW);

                    if (debug_flag)
                    {
                        cout << "---- Camera pos: { " << look_at_pos.pos[0] << ", " << look_at_pos.pos[1] << ", " << zoom_level << " }" << endl;
                        cout << "---- Camera look at pos: { " << look_at_pos.pos[0] << ", " << look_at_pos.pos[1] << ", " << look_at_pos.pos[2] << " }" << endl;
                    }
                }
                else if (debug_flag)
                {
                    zoom_level = 50.0 * VIEWING_DISTANCE_MIN;
                }
            }

            break;
        }
        case CLI_IDENTITY:
        {
            if (!model_stack.empty())
            {
                if (trans_flag == WORLD_COORDS)
                {
                    if (debug_flag)
                        cout << "---- Clear transforms: " << file_name << endl;

                // Clear transformations for object
                    model_stack.top().clearTransformations();
                }
                else if (trans_flag == VIEW_COORDS)
                {
                    if (debug_flag)
                        cout << "---- Clear transforms: view" << endl;

                    Trimesh model = model_stack.top();

                    // Clear transformations for view
                    look_at_pos = model.getCenter();
                    zoom_level = model.getBoundingLength();
                    zoom_level = (zoom_level < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : zoom_level);
                    orbit_theta = orbit_phi = orbit_delta = pan_x = pan_y = 0.0;
                    g_width = win_width;
                    g_height = win_height;

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

                    glMatrixMode(GL_MODELVIEW);

                    if (debug_flag)
                    {
                        cout << "---- Camera pos: { " << look_at_pos.pos[0] << ", " << look_at_pos.pos[1] << ", " << zoom_level << " }" << endl;
                        cout << "---- Camera look at pos: { " << look_at_pos.pos[0] << ", " << look_at_pos.pos[1] << ", " << look_at_pos.pos[2] << " }" << endl;
                    }
                }
            }

            break;
        }
        case CLI_TRANSLATE:
        case CLI_SCALE:
        case CLI_ROTATE:
        {
            float ang = 0.0;
            float xyz[3] = {};

            if (debug_flag)
            {
                if (trans_flag == WORLD_COORDS)
                {
                    if (!model_stack.empty())
                        cout << "---- Transform: " << file_name << endl;
                }
                else if (trans_flag == VIEW_COORDS)
                {
                    cout << "---- Transform: view" << endl;
                }
            }

            // Read angle first if we have a rotate command
            if (command_type == CLI_ROTATE)
            {
                // Skip spaces but make sure not to hit the end of the string 
                while (pos < command_length && command[pos] ==  ' ')
                    pos++;

                if (pos == command_length)
                    return false;

                // Read ANGLE and update position
                angle = "";
                while (pos < command_length && command[pos] != ' ')
                {
                    angle += command[pos++];
                }             

                // Parse ANG from string
                ang = atof(angle.c_str());
            }

            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read X and update position
            x = "";
            while (pos < command_length && command[pos] != ' ')
            {
                x += command[pos++];
            }

            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read Y and update position
            y = "";
            while (pos < command_length && command[pos] != ' ')
            {
                y += command[pos++];
            }

            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read Z and update position
            z = "";
            while (pos < command_length && command[pos] != ' ')
            {
                z += command[pos++];
            }

            // Parse XYZ from string
            xyz[0] = atof(x.c_str());
            xyz[1] = atof(y.c_str());
            xyz[2] = atof(z.c_str());

            if (debug_flag)
            {
                if (command_type == CLI_ROTATE)
                    cout << "---- Angle: " << ang << endl;

                cout << "---- X: " << xyz[0] << endl;
                cout << "---- Y: " << xyz[1] << endl;
                cout << "---- Z: " << xyz[2] << endl;
            }

            if (trans_flag == WORLD_COORDS)
            {
                if (!model_stack.empty())
                {
                    // Add transformation to object
                    if (command_type == CLI_TRANSLATE)
                        model_stack.top().addTransformation(TRANSLATE, xyz);
                    else if (command_type == CLI_SCALE)
                        model_stack.top().addTransformation(SCALE, xyz);
                    else if (command_type == CLI_ROTATE)
                        model_stack.top().addTransformation(ROTATE, xyz, ang);
                }
            }
            else if (trans_flag == VIEW_COORDS)
            {
                // Add transformation to view
                if (command_type == CLI_TRANSLATE)
                {
                    // Pan
                    pan_x += xyz[0];
                    pan_y += xyz[1];
                    zoom_level += -xyz[2];
                }
                else if (command_type == CLI_SCALE)
                {
                    // Zoom
                    // Don't want a divide by zero or objects disappearing
                    if ((xyz[0] == 0.0) || (xyz[1] == 0.0) || (xyz[2] == 0.0))
                        return false;

                    zoom_level *= (xyz[2] * xyz[1] * xyz[0]);
                    g_width *= xyz[0];
                    g_height *= xyz[1];

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

                    glMatrixMode(GL_MODELVIEW);
                }
                else if (command_type == CLI_ROTATE)
                {
                    // Orbit
                    orbit_phi += xyz[0] * ang;
                    orbit_theta -= xyz[1] * ang;
                    orbit_delta += xyz[2] * ang;
                }
            }

            break;
        }
        case CLI_VIEW:
        case CLI_WORLD:
        {
            // Set coordinate system to use
            trans_flag = (command_type == CLI_VIEW ? VIEW_COORDS : WORLD_COORDS);

            if (debug_flag)
                cout << "---- Transformation mode: " << command_type << endl;
            
            break;
        }
        case CLI_MODE:
        {
            // Skip spaces but make sure not to hit the end of the string
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read mode type
            char mode_type = command[pos++];

            // Don't want anybody trying to trick up the CLI
            if (pos < command_length && command[pos] != ' ')
            {
                return false;
            }

            mode_type = toupper(mode_type);

            if (debug_flag)
                cout << "---- Render mode: " << mode_type << endl;

            bool found_mode = false;
            for (int i = 0; i < 6; i++)
            {
                if (found_mode = (mode_type == render_map[i]))
                {
                    curr_render = i;
                    break;
                }
            }

            if (found_mode)
                break;
            else
                return false;
        }
        case CLI_DEBUG:
        {
            // Toggle debug mode
            debug_flag = !debug_flag;

            if (!debug_flag)
                cout << "---- Debug mode: " << debug_flag << endl;
            else
                cout << "Debug mode enabled" << endl;
            
            break;
        }
        case CLI_EXIT:
        {
            if (debug_flag)
                cout << "---- Exit" << endl;

            // Quit program
            exit(EXIT_SUCCESS);
            break;
        }
        default:
            return false;
    }

    return true;
}

// Set camera view depending current zoom, pan, and orbit settings
void SetCameraView(void)
{
    if (!model_stack.empty())
    {
        vertex_t model_center = model_stack.top().getCenter();

        // Zoom
        gluLookAt(look_at_pos.pos[0], look_at_pos.pos[1], zoom_level, look_at_pos.pos[0], look_at_pos.pos[1], look_at_pos.pos[2], 0.0, 1.0, 0.0);

        // Pan
        glTranslatef(-pan_x, -pan_y, 0.0);
        glTranslatef(model_center.pos[0], model_center.pos[1], model_center.pos[2] - zoom_level);

        // Orbit
        glRotatef(orbit_phi, 1.0, 0.0, 0.0);
        glRotatef(orbit_theta, 0.0, 1.0, 0.0);
        glRotatef(orbit_delta, 0.0, 0.0, 1.0);
        glTranslatef(-model_center.pos[0], -model_center.pos[1], -model_center.pos[2]);
    }
    else if (debug_flag)
    {
        // Zoom
        gluLookAt(0.0, 0.0, zoom_level, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        // Pan
        glTranslatef(-pan_x, -pan_y, 0.0);
        glTranslatef(center[0], center[1], center[2] - zoom_level);

        // Orbit
        glRotatef(orbit_phi, 1.0, 0.0, 0.0);
        glRotatef(orbit_theta, 0.0, 1.0, 0.0);
        glTranslatef(-center[0], -center[1], -center[2]);
    }
}

// Setup light position and direction
void SetLighting(void)
{
    // Set light to always be positioned behind the camera
    // g_light_pos[0] = camera_x;
    // g_light_pos[1] = camera_y;
    // g_light_pos[2] = zoom_level;
    // g_light_pos[3] = 1.0;
    // glLightfv(GL_LIGHT0, GL_POSITION, g_light_pos);

    // Set light to always point at whatever the camera is looking at
    g_light_direction[0] = 0.0;
    g_light_direction[1] = 0.0;
    g_light_direction[2] = look_at_pos.pos[2] - zoom_level;
    g_light_direction[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, g_light_direction); 
}

#endif
