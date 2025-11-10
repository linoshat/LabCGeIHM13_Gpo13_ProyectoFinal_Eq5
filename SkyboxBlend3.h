// SkyboxBlend3.h
#pragma once
#include <vector>
#include <string>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader_light.h"   // tu clase Shader

class SkyboxBlend3 {
public:
    SkyboxBlend3();
    // Cada vector DEBE venir en este orden: { +X, -X, +Y, -Y, +Z, -Z }
    bool Init(const std::vector<std::string>& facesDia,
        const std::vector<std::string>& facesMedio,
        const std::vector<std::string>& facesNoche);

    // morningFactor: mezcla Día->Mediodía, nightFactor: mezcla (Día/Mediodía)->Noche
    void Draw(const glm::mat4& view, const glm::mat4& proj,
        float morningFactor, float nightFactor);

private:
    GLuint loadCubemap(const std::vector<std::string>& faces);

    Shader  shader_;               // usa shaders/skybox_mix3.*
    GLuint  vao_ = 0, vbo_ = 0;    // cubo solo con posiciones
    GLuint  texDia_ = 0, texMedio_ = 0, texNoche_ = 0;

    // uniforms comunes de tu clase Shader
    GLint uProj_ = -1, uView_ = -1;

    // uniforms específicos de este shader
    GLint uMorning_ = -1, uNight_ = -1;
    GLint uDaySampler_ = -1, uMidSampler_ = -1, uNightSampler_ = -1;
};
