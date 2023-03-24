#include <application.hpp>
#include <shader.hpp>
#include <iostream>

class AttributesApplication : public our::Application
{

    our::ShaderProgram program;
    GLuint vertex_array = 0, vertex_buffer = 0;

    our::WindowConfiguration getWindowConfiguration() override
    {
        return {"Attributes", {1280, 720}, false};
    }

    void onInitialize() override
    {
        program.create();
        program.attach("assets/shaders/ex05_attributes/attribute_position.vert", GL_VERTEX_SHADER);
        program.attach("assets/shaders/ex02_shader_introduction/red.frag", GL_FRAGMENT_SHADER);
        program.link();

        // vertex_array records the operations on attributes
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);

        glGenBuffers(1, &vertex_buffer);
        // binding is very important before using it (any GL object)
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

//#define USE_GLM_VEC3
#ifndef USE_GLM_VEC3
        float positions[] = {
            -0.5, -0.5, 0.0,
            0.5, -0.5, 0.0,
            0.0, 0.5, 0.0};
#else
        glm::vec3 positions[] = {
            {-0.5, -0.5, 0.0},
            {0.5, -0.5, 0.0},
            {0.0, 0.5, 0.0}};
#endif
        // we tell it by GL_STATIC_DRAW that we don't want to change the data so put it in the far video memory
        glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(float), positions, GL_STATIC_DRAW); // function specifically targeted to copy user-defined data into the currently bound buffer.
                                                                                         // Its first argument is the type of the buffer we want to copy data into:
                                                                                         // the vertex buffer object currently bound to the GL_ARRAY_BUFFER target.
                                                                                         // The second argument specifies the size of the data (in bytes) we want to pass to the buffer;
                                                                                         // a simple sizeof of the vertex data suffices. The third parameter is the actual data we want to send.

        GLuint position_attribute_location = glGetAttribLocation(program, "position");
        std::cout << "Position Attribute Location: " << position_attribute_location << std::endl;
        // Uses GL_ARRAY_BUFFER by default
        // sends 3 floats to each Vertex
        glVertexAttribPointer(position_attribute_location, 3, GL_FLOAT, false, 0, (void *)0);
        glEnableVertexAttribArray(position_attribute_location);

        glBindVertexArray(0); // i finished recording to vertex_array

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }                                                                                                           



    void onDraw(double deltaTime) override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    void onDestroy() override
    {
        program.destroy();
        glDeleteVertexArrays(1, &vertex_array);
        glDeleteBuffers(1, &vertex_buffer);
    }
};

int main(int argc, char **argv)
{
    return AttributesApplication().run();
}
