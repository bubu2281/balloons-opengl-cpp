#include "lab_m1/Tema3/Tema3.h"

#include <vector>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;
using namespace m1;


/*
 *  To find out more about FrameStart, Update, FrameEnd
 *  and the order in which they are called, see world.cpp.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "textures");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "grass_bilboard.png").c_str(), GL_REPEAT);
        mapTextures["grass"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "crate.jpg").c_str(), GL_REPEAT);
        mapTextures["crate"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "earth.png").c_str(), GL_REPEAT);
        mapTextures["earth"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "knit.jpg").c_str(), GL_REPEAT);
        mapTextures["knit"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "heightmap256.png").c_str(), GL_REPEAT);
        mapTextures["heightmap256"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "grass.jpg").c_str(), GL_REPEAT);
        mapTextures["grasstext"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "stone.jpg").c_str(), GL_REPEAT);
        mapTextures["stone"] = texture;
    }

    {
        mapTextures["random"] = CreateRandomTexture(25, 25);
    }

    // Load meshes
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane256");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane256.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a simple quad
    {
        vector<glm::vec3> vertices
        {
            glm::vec3(0.5f,   0.5f, 0.0f),    // top right
            glm::vec3(0.5f,  -0.5f, 0.0f),    // bottom right
            glm::vec3(-0.5f, -0.5f, 0.0f),    // bottom left
            glm::vec3(-0.5f,  0.5f, 0.0f),    // top left
        };

        vector<glm::vec3> normals
        {
            glm::vec3(0, 1, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0)
        };

        vector<glm::vec2> textureCoords
        {
            // TODO(student): Complete texture coordinates for the square
            glm::vec2(0.0f, 0.0f)

        };

        vector<unsigned int> indices =
        {
            0, 1, 3,
            1, 2, 3
        };

        Mesh* mesh = new Mesh("square");
        mesh->InitFromData(vertices, normals, textureCoords, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    for (int i = 0; i < 7; i++) {
        glm::vec3 center((rand() % 10) / 2 - (rand() % 10), 0.0f, (rand() % 10) / 2 - (rand() % 10));
        centers.push_back(center);
        angles.push_back(rand() / 1.0f / RAND_MAX * 2 * M_PI);
    }
}


void Tema3::FrameStart()
{

    glClearColor(0.8f, 0.4f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{
    float angularSpeed = 0.5f; // Viteza de rotație a balonului
    for (int i = 0; i < centers.size(); i++) {
        // Punctul de centru al traiectoriilor (planul XOZ)
        glm::vec3 center = centers[i];
        float angle = angles[i]; // Unghiul pe care balonul îl va roti pe traiectorie

        // Parametrii pentru balon
        float radius = 4.0f; // Raza traiectoriei
        float height = 4.5f + i * 2 + sin(2 * angle); // Înălțimea la care se rotește balonul
        float oscillationAmplitude = 0.2f; // Amplitudinea oscilației pe axa OY
        static float omegaOscillation = 1.0f; // Viteza de oscilație a balonului pe axa OY

        // Calculul traiectoriei concentrice pentru balon
        glm::mat4 modelMatrix = glm::mat4(1);

        // Calculul poziției balonului pe traiectorie circulară
        float xPos = center.x + radius * cos(angle);  // Mișcare pe planul XOZ
        float zPos = center.z + radius * sin(angle);  // Mișcare pe planul XOZ
        float yPos = height + oscillationAmplitude * sin(omegaOscillation * angle); // Oscilația pe axa OY

        modelMatrix = glm::translate(modelMatrix, glm::vec3(xPos, yPos, zPos));

        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, 0, mapTextures["knit"]);


        glm::mat4 nacelMatrix = modelMatrix;
        nacelMatrix = glm::translate(nacelMatrix, glm::vec3(0.0f, -1.0f, 0.0f));
        nacelMatrix = glm::scale(nacelMatrix, glm::vec3(0.4f, 0.2f, 0.4f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], nacelMatrix, 0, mapTextures["crate"]);

        glm::mat4 connectorMatrix = modelMatrix;
        connectorMatrix = glm::scale(connectorMatrix, glm::vec3(0.05f, 0.5f, 0.05f));

        glm::mat4 connector1 = connectorMatrix;
        connector1 = glm::translate(connector1, glm::vec3(3.9f, -1.5f, -3.7f));
        connector1 = glm::rotate(connector1, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], connector1, 0, mapTextures["crate"]);

        glm::mat4 connector2 = connectorMatrix;
        connector2 = glm::translate(connector2, glm::vec3(-3.8f, -1.5f, -3.7f));
        connector2 = glm::rotate(connector2, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], connector2, 0, mapTextures["crate"]);

        glm::mat4 connector3 = connectorMatrix;
        connector3 = glm::translate(connector3, glm::vec3(-3.8f, -1.5f, 3.7f));
        connector3 = glm::rotate(connector3, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], connector3, 0, mapTextures["crate"]);

        glm::mat4 connector4 = connectorMatrix;
        connector4 = glm::translate(connector4, glm::vec3(3.9f, -1.5f, 3.7f));
        connector4 = glm::rotate(connector4, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], connector4, 0, mapTextures["crate"]);

        // Actualizăm unghiul pentru rotație 
        if (angle > glm::two_pi<float>()) {
            angle -= glm::two_pi<float>();  // Facem rotația periodică, între 0 și 2*pi
        }
        angles[i] += angularSpeed * deltaTimeSeconds;
    }
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(45.0f, 1.0f, 45.0f));
    RenderSimpleMesh(meshes["plane256"], shaders["LabShader"], modelMatrix, 1, mapTextures["heightmap256"], mapTextures["grasstext"], mapTextures["stone"]);
}


void Tema3::FrameEnd()
{
    DrawCoordinateSystem();
}

void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int is_terrain, Texture2D* texture1, Texture2D* texture2, Texture2D* texture3)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    GLint loc_object_is_terrain = glGetUniformLocation(shader->program, "object_is_terrain");
    glUniform1i(loc_object_is_terrain, is_terrain);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // TODO(student): Set any other shader uniforms that you need

    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        // - bind the texture1 ID
        // - send theuniform value
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0); // Send to shader

    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        // - bind the texture2 ID
        // - send the uniform value
        glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture2"), 1); // Send to shader

    }

    if (texture3)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        // - bind the texture2 ID
        // - send the uniform value
        glActiveTexture(GL_TEXTURE2); // Activate texture unit 1
        glBindTexture(GL_TEXTURE_2D, texture3->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture3"), 2); // Send to shader

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
    GLuint textureID = 0;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    unsigned char* data = new unsigned char[size];

    // TODO(student): Generate random texture data

    // TODO(student): Generate and bind the new texture ID

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see input_controller.h.
 */


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    }
}


void Tema3::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
