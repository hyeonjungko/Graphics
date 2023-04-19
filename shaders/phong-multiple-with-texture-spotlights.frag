#version 330

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec3 spotDirection; // added for spotlight support
    float spotAngle;    // added for spotlight support
    bool isSpotlight;
};


in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord; //

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light[MAXLIGHTS];
uniform int numLights;

/* texture */
uniform sampler2D image;

out vec4 fColor;

vec4 calcLight(LightProperties light) {
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;

    if (light.position.w!=0)
        lightVec = normalize(light.position.xyz - fPosition.xyz);
    else
        lightVec = normalize(-light.position.xyz);
    vec3 tNormal = fNormal;
    normalView = normalize(tNormal.xyz);
    nDotL = dot(normalView,lightVec);
    viewVec = -fPosition.xyz;
    viewVec = normalize(viewVec);
    reflectVec = reflect(-lightVec,normalView);
    reflectVec = normalize(reflectVec);
    rDotV = max(dot(reflectVec,viewVec),0.0);
    ambient = material.ambient * light.ambient;
    diffuse = material.diffuse * light.diffuse * max(nDotL,0);
    if (nDotL>0)
        specular = material.specular * light.specular * pow(rDotV,material.shininess);
    else
        specular = vec3(0,0,0);

    return vec4(ambient + diffuse + specular, 1.0);           
}

void main()
{
    float dDotMinusL;

    fColor = vec4(0,0,0,1);

    for (int i=0;i<numLights;i++)
    {
        if (light[i].isSpotlight) {

            vec3 l = normalize(light[i].position.xyz - fPosition.xyz);
            dDotMinusL = dot(normalize(light[i].spotDirection), -l);
            if (dDotMinusL > light[i].spotAngle) { 
                vec4 color = calcLight(light[i]);

                fColor = fColor + color;
            }
        }
        else {
            fColor = fColor + calcLight(light[i]);       
        }
    }
    // fColor = vec4(fTexCoord.s,fTexCoord.t,0,1); //
    //fColor = texture(image,fTexCoord.st);
    fColor = fColor * texture(image,fTexCoord.st);
}
