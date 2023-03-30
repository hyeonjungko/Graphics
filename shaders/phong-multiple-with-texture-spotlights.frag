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
in vec4 fTexCoord;

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
        ///////////////////////////////
        // NEW & IMPROVED 

        if (light[i].isSpotlight) {
            // //
            // vec3 lightVec,viewVec,reflectVec;
            // vec3 normalView;
            // vec3 ambient,diffuse,specular;
            // float nDotL,rDotV;

            // if (light[i].position.w!=0)
            //     lightVec = normalize(light[i].position.xyz - fPosition.xyz);
            // else
            //     lightVec = normalize(-light[i].position.xyz);
            // vec3 tNormal = fNormal;
            // normalView = normalize(tNormal.xyz);
            // nDotL = dot(normalView,lightVec);
            // viewVec = -fPosition.xyz;
            // viewVec = normalize(viewVec);
            // reflectVec = reflect(-lightVec,normalView);
            // reflectVec = normalize(reflectVec);
            // rDotV = max(dot(reflectVec,viewVec),0.0);
            // ambient = material.ambient * light[i].ambient;
            // diffuse = material.diffuse * light[i].diffuse * max(nDotL,0);
            // if (nDotL>0)
            //     specular = material.specular * light[i].specular * pow(rDotV,material.shininess);
            // else
            //     specular = vec3(0,0,0);
            // //
            vec3 l = normalize(light[i].position.xyz - fPosition.xyz);
            dDotMinusL = dot(normalize(light[i].spotDirection), -l);
            if (dDotMinusL > light[i].spotAngle) { 
                vec4 color = calcLight(light[i]);
                //float spotLightIntensity = (1.0 - (1.0 - dDotMinusL)/(1.0 - light[i].spotAngle));

                fColor = fColor + color;
                // fColor = color;
                //fColor = fColor + vec4(ambient + spotLightIntensity * (diffuse + specular), 1.0); //
                // fColor = fColor + color * spotLightIntensity;
            }
        }
        else {
            fColor = fColor + calcLight(light[i]);       
        }
        ///////////////////////////////
    }
    // fColor = texture(image,fTexCoord.st);
    fColor = fColor * texture(image,fTexCoord.st);
    // fColor = vec4(fTexCoord.s,fTexCoord.t,0,1);

    //////
    // lightingShader.setVec3("light.position", camera.Position); lightingShader.setVec3("light.direction", camera.Front);
    // ////////////
    // float cosTheta = dot(spotDir, -lightDir);
    // float spotFactor = pow(cosTheta, exponent);
    // if (cosTheta < cos(radians(cutoffAngle)))
    // {
    //     spotFactor = 0.0;
    // }
    // vec3 spotDir = normalize(vec3(gl_ModelViewMatrix * lightDirection));
    // float cosTheta = dot(spotDir, -L);
    // float spotFactor = pow(max(cosTheta, 0.0), spotExponent) * step(cosTheta, cos(radians(spotCutoff)));
    // float diffuse = max(dot(N, L), 0.0);
    // float specular = pow(max(dot(N, H), 0.0), 32.0);
    // vec3 color = lightColor + lightIntensity + (diffuse + specular) * spotFactor;
    
    // color vec4 diffuseColor = max(dot(vNormal, lightDir), 0.0) * lightColor;
    // vec3 reflectDir = reflect(-lightDir, vNormal);
    // float specularFactor = pow(max(dot(reflectDir, vViewDir), 0.0), gl_FrontMaterial.shininess);
    // vec4 specularColor = specularFactor * lightColor;
    // gl_FragColor = mix(diffuseColor, specularColor, gl_FrontMaterial.specular) * spotFactor;
}
