#version 120
uniform sampler2D SourceTexture;
void main()
{
    vec3 color = texture2D(SourceTexture, gl_TexCoord[0].st).rgb;
    float luminance = dot(color, vec3(0.2125, 0.7154, 0.0721));
    float delta = 0.01;
    gl_FragColor = vec4(luminance + delta);
}
