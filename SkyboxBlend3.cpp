// SkyboxBlend3.cpp
#include "SkyboxBlend3.h"
#include "stb_image.h"   // ¡no definir STB_IMAGE_IMPLEMENTATION aquí!
#include <cstdio>

SkyboxBlend3::SkyboxBlend3() {}

bool SkyboxBlend3::Init(const std::vector<std::string>& facesDia,
    const std::vector<std::string>& facesMedio,
    const std::vector<std::string>& facesNoche)
{
    // 1) Shader
    shader_.CreateFromFiles("shaders/skybox_mix3.vert", "shaders/skybox_mix3.frag");
    uProj_ = shader_.GetProjectionLocation();
    uView_ = shader_.GetViewLocation();

    // Usa el getter nuevo del Shader
    GLuint pid = shader_.GetProgramID();
    uMorning_ = glGetUniformLocation(pid, "uMorningFactor");
    uNight_ = glGetUniformLocation(pid, "uNightFactor");
    uDaySampler_ = glGetUniformLocation(pid, "uCubemapDia");
    uMidSampler_ = glGetUniformLocation(pid, "uCubemapMedio");
    uNightSampler_ = glGetUniformLocation(pid, "uCubemapNoche");

    // 2) Cargar cubemaps
    texDia_ = loadCubemap(facesDia);
    texMedio_ = loadCubemap(facesMedio);
    texNoche_ = loadCubemap(facesNoche);
    if (!texDia_ || !texMedio_ || !texNoche_) return false;

    // 3) Cubo (36 vértices)
    const float verts[] = {
        // back (-Z)
        -1,-1,-1,  1,-1,-1,  1, 1,-1,
        -1,-1,-1,  1, 1,-1, -1, 1,-1,
        // front (+Z)
        -1,-1, 1,  1, 1, 1,  1,-1, 1,
        -1,-1, 1, -1, 1, 1,  1, 1, 1,
        // left (-X)
        -1,-1,-1, -1, 1,-1, -1, 1, 1,
        -1,-1,-1, -1, 1, 1, -1,-1, 1,
        // right (+X)
         1,-1,-1,  1,-1, 1,  1, 1, 1,
         1,-1,-1,  1, 1, 1,  1, 1,-1,
         // bottom (-Y)
         -1,-1,-1, -1,-1, 1,  1,-1, 1,
         -1,-1,-1,  1,-1, 1,  1,-1,-1,
         // top (+Y)
         -1, 1,-1,  1, 1,-1,  1, 1, 1,
         -1, 1,-1,  1, 1, 1, -1, 1, 1
    };
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    return true;
}

GLuint SkyboxBlend3::loadCubemap(const std::vector<std::string>& faces) {
    if (faces.size() != 6) return 0;

    GLuint texID; glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    int w, h, comp;
    stbi_set_flip_vertically_on_load(false);
    for (GLuint i = 0; i < 6; ++i) {
        const std::string& f = faces[i];
        unsigned char* data = stbi_load(f.c_str(), &w, &h, &comp, 0);
        if (!data) { printf("Skybox: no pude cargar cara: %s\n", f.c_str()); return 0; }
        GLenum fmt = (comp == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

void SkyboxBlend3::Draw(const glm::mat4& view, const glm::mat4& proj,
    float morningFactor, float nightFactor)
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    shader_.UseShader();

    // matrices (sin traslación en la vista)
    glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(uProj_, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(uView_, 1, GL_FALSE, glm::value_ptr(viewNoTrans));

    glUniform1f(uMorning_, morningFactor);
    glUniform1f(uNight_, nightFactor);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, texDia_);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_CUBE_MAP, texMedio_);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_CUBE_MAP, texNoche_);
    glUniform1i(uDaySampler_, 0);
    glUniform1i(uMidSampler_, 1);
    glUniform1i(uNightSampler_, 2);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}
