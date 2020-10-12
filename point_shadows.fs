#version 330 core
out vec4 FragColor;

in VS_OUT {
   vec3 FragPos;
   vec2 TexCoords;
   mat3 TBN;
   vec3 Normal;
} fs_in;

struct Material {
    sampler2D diffuseMap1;
    sampler2D specularMap1;
    sampler2D normalMap1;
}; 



struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform samplerCube depthMap;
uniform PointLight pointLight;

uniform vec3 viewPos;

uniform float far_plane;


vec3 TangentViewPos;
vec3 TangentFragPos;
vec3 TangentLightPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color);

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - pointLight.position;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void main()
{           
    
    vec3 normal = texture(material.normalMap1, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0); 

    vec3 color = texture(material.diffuseMap1, fs_in.TexCoords).rgb;

    TangentViewPos= fs_in.TBN * viewPos;
    TangentFragPos= fs_in.TBN * fs_in.FragPos;

    vec3 viewDir =normalize(TangentViewPos - TangentFragPos);

    vec3 result = CalcPointLight(pointLight, normal, fs_in.FragPos, viewDir, color); 

    FragColor = vec4(result, 1.0);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color)
{
    vec3 TangentLightPos=fs_in.TBN * light.position;
    
    vec3 lightDir =normalize(TangentLightPos - TangentFragPos);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse =light.diffuse* diff * color;

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * color;
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap1, fs_in.TexCoords));


    float shadow = ShadowCalculation(fragPos);

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}
