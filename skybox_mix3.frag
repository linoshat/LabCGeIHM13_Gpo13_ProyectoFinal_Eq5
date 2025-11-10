#version 330 core
in vec3 vDir;
out vec4 FragColor;

uniform samplerCube uCubemapDia;
uniform samplerCube uCubemapMedio;
uniform samplerCube uCubemapNoche;

uniform float uMorningFactor;  // 0..1  (Dia->Medio)
uniform float uNightFactor;    // 0..1  ((Dia/Medio)->Noche)

void main() {
    vec3 dir = normalize(vDir);
    vec3 cDay   = texture(uCubemapDia,   dir).rgb;
    vec3 cMid   = texture(uCubemapMedio, dir).rgb;
    vec3 cNight = texture(uCubemapNoche, dir).rgb;

    vec3 m1 = mix(cDay, cMid, clamp(uMorningFactor, 0.0, 1.0));
    vec3 m2 = mix(m1,   cNight, clamp(uNightFactor,   0.0, 1.0));

    FragColor = vec4(m2, 1.0);
}
