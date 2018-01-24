#version 120
uniform sampler2D DiffuseTexture;
uniform vec3 LightPosition;
uniform vec4 LightColor;
varying vec3 WorldNormal;
varying vec3 WorldPosition;
void main()
{
	vec4 ambient = 0.2 * vec4(0.7, 0.6, 0.7, 1.0);
    vec4 color = vec4(texture2D(DiffuseTexture, gl_TexCoord[0].st).rgb, 1.0);
    vec3 fragToLight = normalize(LightPosition - WorldPosition);
    float coefficient = clamp(dot(WorldNormal, fragToLight), 0.0, 1.0);
    gl_FragColor = coefficient * LightColor * color + ambient;
}
