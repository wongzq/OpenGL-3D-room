#version 330 core

uniform vec3 vColor;
uniform vec3 lightPos1;
uniform vec3 lightColor1;
uniform vec3 lightPos2;
uniform vec3 lightColor2;
uniform vec3 ambient;
uniform vec3 viewPos;	//(camX, camY, camZ)
uniform int choice;

out vec4 fragColor;
in vec3 vNormal;
in vec3 vPos;

void main()
{
    //Diffuse light component calculation
	vec3 norm = normalize(vNormal);
	vec3 lightDir1 = normalize (lightPos1 - vPos);
	vec3 lightDir2 = normalize (lightPos2 - vPos);
	vec3 diffuse1 = max (dot(norm, lightDir1), 0.0) * lightColor1;
	vec3 diffuse2 = max (dot(norm, lightDir2), 0.0) * lightColor2;
	
	//Specular light component calculation
	vec3 viewDir = normalize(viewPos - vPos);
	vec3 reflectDir1 = reflect(-lightDir1, norm);
	vec3 reflectDir2 = reflect(-lightDir2, norm);
	vec3 specular1 = pow(max(dot(viewDir, reflectDir1), 0.0), 20) * lightColor1;
	vec3 specular2 = pow(max(dot(viewDir, reflectDir2), 0.0), 20) * lightColor2;
	
	if (choice == 0)
		fragColor = vec4 ((ambient + diffuse1 + diffuse2 + specular1 + specular2) * vColor, 1.0);
	else if (choice == 1)
		fragColor = vec4 ((ambient + diffuse1 + specular1) * vColor, 1.0);
	else if (choice == 2)
		fragColor = vec4 ((ambient + diffuse2 + specular2) * vColor, 1.0);

} 