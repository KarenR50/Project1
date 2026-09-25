#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

uniform sampler2D tex0;
uniform sampler2D normalMap;

void main()
{
   //obtener el mapa de normales
    vec3 normalMapValue = texture(normalMap, texCoord).rgb;

    // Convertir de [0,1] a [-1,1]
    normalMapValue = normalMapValue * 2.0 - 1.0;

//matriz btn

    mat3 TBN = mat3(
        normalize(Tangent),
        normalize(Bitangent),
        normalize(Normal)
    );


  //obtener normal final

    vec3 normal = normalize(TBN * normalMapValue);

//iluminacion

    vec3 lightDir = normalize(vec3(0.0, 0.5 , 1.0));

    float diffuse = max(dot(normal, lightDir), 0.0);

    float ambient = 0.2;

    float lighting = ambient + diffuse;


   //textura + iluminacion

    vec4 texColor = texture(tex0, texCoord);

    FragColor = texColor * lighting;
}