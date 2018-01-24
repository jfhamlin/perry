#version 120

vec3 lerp(vec3 x0, vec3 x1, float alpha)
{
    return x0 + alpha * (x1 - x0);
}

uniform sampler2D SceneTexture;
uniform vec3 ColorFilter;
uniform float Brightness;
uniform float Saturation;
void main()
{
    vec4 sceneColor = texture2D(SceneTexture, gl_TexCoord[0].st);
    float luminance = dot(sceneColor.rgb, vec3(0.3, 0.59, 0.11));
    vec3 color = ColorFilter * sceneColor.rgb;
    vec3 finalColor = lerp(vec3(luminance), color, Saturation);
    finalColor *= Brightness;
    gl_FragColor = vec4(finalColor, sceneColor.a);
}
