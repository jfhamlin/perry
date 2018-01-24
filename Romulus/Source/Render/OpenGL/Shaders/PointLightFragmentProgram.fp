#version 120

float lerp(float x0, float x1, float alpha)
{
    return x0 + alpha * (x1 - x0);
}

vec4 PCF33(sampler2DShadow shadowMap, vec3 pos, float minX, float maxX,
             float minY, float maxY, float texelWidth, float texelHeight)
{
    // Try to get something like a jittered set of samples.
    float xleft = max(pos.x - texelWidth, minX);
    float xleftleft = max(pos.x - 2.0 * texelWidth, minX);

    float xright = min(pos.x + texelWidth, maxX);
    float xrightright = min(pos.x + 2.0 * texelWidth, maxX);

    float ybottom = max(pos.y - texelHeight, minY);
    float ybottombottom = max(pos.y - 2.0 * texelHeight, minY);

    float ytop = min(pos.y + texelHeight, maxY);
    float ytoptop = min(pos.y + 2.0 * texelHeight, maxY);


    vec4 sample0 = shadow2D(shadowMap, pos);

    vec4 sample1 = shadow2D(shadowMap, vec3(xleftleft, ytop, pos.z));
    vec4 sample2 = shadow2D(shadowMap, vec3(xleft, ybottombottom, pos.z));
    vec4 sample3 = shadow2D(shadowMap, vec3(xright, ytoptop, pos.z));
    vec4 sample4 = shadow2D(shadowMap, vec3(xrightright, ybottom, pos.z));

    vec4 sample5 = shadow2D(shadowMap, vec3(xleft, ytop, pos.z));
    vec4 sample6 = shadow2D(shadowMap, vec3(xright, pos.yz));
    vec4 sample7 = shadow2D(shadowMap, vec3(xleftleft, ybottom, pos.z));
    vec4 sample8 = shadow2D(shadowMap, vec3(xrightright, ytop, pos.z));
    return (1.0 / 9.0) * (sample0 + sample1 + sample2 + sample3 + sample4 +
                          sample5 + sample6 + sample7 + sample8);
}

uniform float WindowWidth;
uniform float WindowHeight;
uniform sampler2D GBuffer1;
uniform sampler2D GBuffer2;
uniform sampler2D GBuffer3;
uniform sampler2D LightAccumulation;
uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform vec3 ViewerPosition;
uniform float LightIntensity;
uniform float FarAttenuation;
uniform mat4 LightViewTransform;
uniform sampler2DShadow FrontAndBackShadowMap;
uniform float ShadowMapSize;
void main()
{
    vec2 st = vec2(gl_FragCoord.x / WindowWidth, gl_FragCoord.y / WindowHeight);
    vec3 position = texture2D(GBuffer3, st).xyz;

    // Projection into light space for shadowing.
    vec4 lightSpacePosition = LightViewTransform * vec4(position, 1.0);
    float zSign = sign(lightSpacePosition.z);
    // Make zSign either -1 or -1; 1 if zSign == 0.
    zSign = clamp(2.0 * zSign - 1.0, -1.0, 1.0);
    lightSpacePosition /= lightSpacePosition.w;
    float l = length(lightSpacePosition.xyz);
    lightSpacePosition /= l;
    lightSpacePosition.z *= zSign;
    lightSpacePosition.z += 1.0;
    lightSpacePosition.x /= lightSpacePosition.z;
    lightSpacePosition.y /= lightSpacePosition.z;
    lightSpacePosition.z = l / FarAttenuation;
    lightSpacePosition.xy =  .5 * (vec2(-zSign * lightSpacePosition.x,
                                        lightSpacePosition.y) + 1.0);
    // Since each shadow map takes up half of the width.
    lightSpacePosition.x *= .5;
    // The front shadow map is in the left half of the FrontAndBackShadowMap.
    float backMapOffset = .5 * clamp(zSign, 0.0, 1.0);
    lightSpacePosition.x += backMapOffset;
    vec4 shadowModulation = PCF33(FrontAndBackShadowMap,
                                  lightSpacePosition.xyz,
                                  backMapOffset, 0.5 + backMapOffset,
                                  0.0, 1.0,
                                  0.5 / ShadowMapSize, 1.0 / ShadowMapSize);

    // Calculate lighting contribution.
    vec3 toLight = normalize(LightPosition - position);
    vec3 toViewer = normalize(ViewerPosition - position);
    vec3 normal = texture2D(GBuffer2, st).xyz;
    float normalDotLight = dot(normal, toLight);
    vec3 reflect = normalize(2.0 * normal * (normalDotLight) - toLight);
    float diffuseIntensity = max(0.0, normalDotLight);
    vec3 diffuseAlbedo = texture2D(GBuffer1, st).xyz;
    vec3 diffuseComponent = (diffuseAlbedo * LightColor) * diffuseIntensity;
    float specularExponent = texture2D(GBuffer2, st).a;
    float specularIntensity = pow(max(0.0, dot(reflect, toViewer)),
                                  specularExponent);
    float specularAlbedo = texture2D(GBuffer1, st).a;
    vec3 specularComponent =
            specularIntensity * specularAlbedo * LightColor;
    float attenuation = lerp(1.0, 0.0,
                             min(1.0, length(LightPosition - position) /
                                      FarAttenuation));
    vec3 lightContribution = (diffuseComponent + specularComponent) *
                             LightIntensity * attenuation;

    vec3 previousLight = texture2D(LightAccumulation, st).xyz;
    gl_FragColor = vec4(shadowModulation.rgb * lightContribution +
                        previousLight, 1.0);
}
