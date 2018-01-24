#version 120
uniform sampler2D SceneTexture;
uniform float Key;
uniform float WhiteLuminanceSquared;
uniform float AverageLuminance;
uniform int Level;
void main()
{
    vec4 color = texture2D(SceneTexture, gl_TexCoord[0].st);
    float luminance = dot(color.rgb, vec3(0.2125, 0.7154, 0.0721)) + 0.01;
    float scaledLuminance = (Key / AverageLuminance) * luminance;
    scaledLuminance = (scaledLuminance * (1.0 +
                                          (scaledLuminance /
                                           WhiteLuminanceSquared))) /
                      (1 + scaledLuminance);
    float factor = scaledLuminance / luminance;
    gl_FragColor = vec4(color.rgb * factor, color.a);
}
