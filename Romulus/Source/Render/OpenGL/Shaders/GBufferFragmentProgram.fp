#version 120
#extension GL_ARB_draw_buffers : enable
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;
uniform float SpecularAlbedo;
uniform float SpecularExponent;
varying vec3 WorldNormal;
varying vec3 WorldTangent;
varying vec3 WorldPosition;
void main()
{
    vec3 worldBitangent = cross(normalize(WorldTangent),
                                normalize(WorldNormal));
    vec3 normal = texture2D(NormalTexture, gl_TexCoord[0].st).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    mat3 tbn = mat3(WorldTangent, worldBitangent, WorldNormal);
    vec3 worldNormal = normalize(tbn * normal);
    gl_FragData[0] = vec4(texture2D(DiffuseTexture, gl_TexCoord[0].st).rgb,
                          SpecularAlbedo);
    gl_FragData[1] = vec4(worldNormal, SpecularExponent);
    gl_FragData[2] = vec4(WorldPosition, 0);
}
