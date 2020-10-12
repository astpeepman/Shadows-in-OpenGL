#version 330 core
out vec4 FragColor;

in VS_OUT {
   vec3 FragPos;
   vec2 TexCoords;
   mat3 TBN;
   vec4 FragPosLightSpace;
   vec3 Normal;
} fs_in;

struct Material {
    sampler2D diffuseMap1;
    sampler2D specularMap1;
    sampler2D normalMap1;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


uniform sampler2D shadowMap;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material material;


vec3 TangentViewPos;
vec3 TangentFragPos;
vec3 TangentLightPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
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

   vec3 result=CalcDirLight(dirLight, normal, viewDir, color);

   //result += CalcPointLight(pointLights, normal, fs_in.FragPos, viewDir, color); 

   FragColor = vec4(result, 1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color)
{
//ambient
    vec3 ambient = light.ambient * color;
    //diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * color;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap1, fs_in.TexCoords));    

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir);                 
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    return lighting;
   
}
