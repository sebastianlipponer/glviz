// This file is part of GLviz.
//
// Copyright(c) 2014-2018 Sebastian Lipponer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <GLviz>

#include "config.hpp"

#include <Eigen/Core>

#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <exception>
#include <cmath>

using namespace Eigen;

namespace
{

GLviz::Camera g_camera;

float g_time(0.0f);
bool g_stop_simulation(true);

bool g_enable_mesh3(true);
bool g_enable_wireframe(false);

bool g_enable_points(false);
float g_point_radius(0.0014f);
float g_projection_radius(0.0f);

float g_wireframe[4] = {
    0.0f, 0.0f, 0.0f, 1.0f
};

float g_mesh_material[4] = {
    0.0f, 0.25f, 1.0f, 8.0f
};

float g_points_material[4] = {
    1.0f, 1.0f, 1.0f, 8.0f
};

int g_shading_method(0);

struct MyViz
{
    MyViz()
    {
        // Setup vertex array v.
        vertex_array_v.bind();

        vertex_array_buffer.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
        vertex_array_v.unbind();

        // Setup vertex array vf.
        vertex_array_vf.bind();

        vertex_array_buffer.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

        index_array_buffer.bind();
        vertex_array_buffer.unbind();

        vertex_array_vf.unbind();

        // Setup vertex array vnf.
        vertex_array_vnf.bind();
        
        vertex_array_buffer.bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

        normal_array_buffer.bind();
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

        index_array_buffer.bind();
        vertex_array_buffer.unbind();

        vertex_array_vnf.unbind();

        // Bind uniforms to their binding points.
        uniform_camera.bind_buffer_base(0);
        uniform_material.bind_buffer_base(1);
        uniform_wireframe.bind_buffer_base(2);
        uniform_sphere.bind_buffer_base(3);

        g_camera.translate(Eigen::Vector3f(0.0f, 0.0f, -2.0f));
    }

    void draw_mesh3(GLsizei nf)
    {
        program_mesh3.use();

        if (g_shading_method == 0)
        {
            // Flat.
            vertex_array_vf.bind();
            glDrawElements(GL_TRIANGLES, nf,
                GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
            vertex_array_vf.unbind();
        }
        else
        {
            // Smooth.
            vertex_array_vnf.bind();
            glDrawElements(GL_TRIANGLES, nf,
                GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));
            vertex_array_vnf.unbind();
        }

        program_mesh3.unuse();
    }

    void draw_spheres(GLsizei nv)
    {
        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

        program_sphere.use();

        vertex_array_v.bind();
        glDrawArrays(GL_POINTS, 0, nv);
        vertex_array_v.unbind();

        program_sphere.unuse();
    }

    GLviz::glVertexArray  vertex_array_v, vertex_array_vf, vertex_array_vnf;
    GLviz::glArrayBuffer  vertex_array_buffer, normal_array_buffer;
    GLviz::glElementArrayBuffer  index_array_buffer;

    GLviz::UniformBufferCamera      uniform_camera;
    GLviz::UniformBufferMaterial    uniform_material;
    GLviz::UniformBufferWireframe   uniform_wireframe;
    GLviz::UniformBufferSphere      uniform_sphere;

    GLviz::ProgramMesh3   program_mesh3;
    GLviz::ProgramSphere  program_sphere;
};

std::unique_ptr<MyViz> viz;

std::vector<Eigen::Vector3f>               g_ref_vertices;
std::vector<Eigen::Vector3f>               g_ref_normals;

std::vector<Eigen::Vector3f>               g_vertices;
std::vector<Eigen::Vector3f>               g_normals;
std::vector<std::array<unsigned int, 3> >  g_faces;

void load_triangle_mesh(std::string const& filename);

void
display()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viz->vertex_array_buffer.set_buffer_data(3 * sizeof(GLfloat)
        * g_vertices.size(), g_vertices.front().data());
    viz->normal_array_buffer.set_buffer_data(3 * sizeof(GLfloat)
        * g_normals.size(), g_normals.front().data());
    viz->index_array_buffer.set_buffer_data(3 * sizeof(GLuint)
        * g_faces.size(), g_faces.front().data());

    viz->uniform_camera.set_buffer_data(g_camera);

    if (g_enable_mesh3)
    {
        viz->uniform_material.set_buffer_data(g_mesh_material);
        
        viz->program_mesh3.set_wireframe(g_enable_wireframe);
        int screen[2] = { GLviz::screen_width(), GLviz::screen_height() };
        viz->uniform_wireframe.set_buffer_data(g_wireframe, screen);        

        viz->program_mesh3.set_smooth(g_shading_method != 0);
        viz->draw_mesh3(static_cast<GLsizei>(3 * g_faces.size()));
    }

    if (g_enable_points)
    {
        viz->uniform_material.set_buffer_data(g_points_material);
        GLviz::Frustum view_frustum = g_camera.get_frustum();
        float g_projection_radius = view_frustum.near_() * (GLviz::screen_height()
            / (view_frustum.top() - view_frustum.bottom()));

        viz->uniform_sphere.set_buffer_data(g_point_radius, g_projection_radius);

        viz->draw_spheres(static_cast<GLsizei>(g_vertices.size()));
    }
}

void
reshape(int width, int height)
{
    const float aspect = static_cast<float>(width) /
        static_cast<float>(height);

    glViewport(0, 0, width, height);
    g_camera.set_perspective(60.0f, aspect, 0.005f, 5.0f);
}

void
timer(int delta_t_msec)
{
    float delta_t_sec = static_cast<float>(delta_t_msec) / 1000.0f;

    if (!g_stop_simulation)
    {
        g_time += delta_t_sec;

        const float k = 50.0f;
        const float a = 0.03f;
        const float v = 10.0f;

        for (unsigned int i(0); i < g_vertices.size(); ++i)
        {
            const float x = g_ref_vertices[i].x() + g_ref_vertices[i].y()
                + g_ref_vertices[i].z();

            const float u = 5.0f * (x - 0.75f * std::sin(2.5f * g_time));
            const float w = (a / 2.0f) * (1.0f
                + std::sin(k * x + v * g_time));

            g_vertices[i] = g_ref_vertices[i] + (std::exp(-u * u) * w)
                * g_ref_normals[i];
        }

        GLviz::set_vertex_normals_from_triangle_mesh(
            g_vertices, g_faces, g_normals);
    }
}

void
close()
{
    viz = nullptr;
}

void
gui()
{
    ImGui::Begin("GLviz", nullptr);
    ImGui::SetWindowPos(ImVec2(3.0f, 3.0f), ImGuiCond_Once);
    ImGui::SetWindowSize(ImVec2(265.0f, 345.0f), ImGuiCond_Once);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.55f);

    ImGui::Text("time\t %.3f", g_time);
    ImGui::Text("fps \t %.1f fps", ImGui::GetIO().Framerate);

    if (ImGui::CollapsingHeader("Mesh", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Draw Triangle Mesh", &g_enable_mesh3);
        ImGui::ColorEdit3("Mesh Color", g_mesh_material);
        ImGui::DragFloat("Mesh Shininess", &g_mesh_material[3],
            1e-2f, 1e-12f, 1000.0f);
        ImGui::Combo("Mesh Shading", &g_shading_method, "Flat\0Phong\0");

        ImGui::Separator();

        ImGui::Checkbox("Draw Wireframe", &g_enable_wireframe);
        ImGui::ColorEdit3("Wireframe Color", g_wireframe);
    }

    if (ImGui::CollapsingHeader("Points", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Draw Points", &g_enable_points);
        ImGui::DragFloat("Points Radius", &g_point_radius,
            1e-5f, 0.0f, 0.1f, "%.4f");
        ImGui::ColorEdit3("Points Color", g_points_material);
        ImGui::DragFloat("Points Shininess", &g_points_material[3],
            1e-2f, 1e-12f, 1000.0f);
    }

    ImGui::End();
}

void
keyboard(SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_r:    g_time = 0.0f;                             break;
        case SDLK_SPACE:  g_stop_simulation = !g_stop_simulation;  break;
        case SDLK_1:    g_enable_mesh3 = !g_enable_mesh3;          break;
        case SDLK_2:    g_enable_points = !g_enable_points;        break;
        case SDLK_5:    g_shading_method = (g_shading_method + 1) % 2; break;
        case SDLK_w:    g_enable_wireframe = !g_enable_wireframe;  break;
    }
}

void
load_triangle_mesh(std::string const& filename)
{
    std::cout << "\nRead " << filename << "." << std::endl;
    std::ifstream input(filename);

    if (input.good())
    {
        input.close();
        GLviz::load_raw(filename, g_vertices, g_faces);
    }
    else
    {
        input.close();

        std::ostringstream fqfn;
        fqfn << path_resources;
        fqfn << filename;
        GLviz::load_raw(fqfn.str(), g_vertices, g_faces);
    }

    std::cout << "  #vertices " << g_vertices.size() << std::endl;
    std::cout << "  #faces    " << g_faces.size() << std::endl;

    GLviz::set_vertex_normals_from_triangle_mesh(
        g_vertices, g_faces, g_normals);

    g_ref_vertices = g_vertices;
    g_ref_normals = g_normals;
}

}

int
main(int argc, char* argv[])
{
    GLviz::GLviz();
  
    viz = std::unique_ptr<MyViz>(new MyViz());
    
    try
    {
        load_triangle_mesh("stanford_dragon_v40k_f80k.raw");
    }
    catch(std::runtime_error const& e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    GLviz::display_callback(display);
    GLviz::reshape_callback(reshape);
    GLviz::timer_callback(timer, 15);
    GLviz::close_callback(close);
    GLviz::gui_callback(gui);
    GLviz::keyboard_callback(keyboard);

    return GLviz::exec(g_camera);
}
