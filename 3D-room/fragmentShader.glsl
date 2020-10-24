#version 330 core

uniform vec3 vColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
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
	vec3 lightDir = normalize (lightPos - vPos);
					//if vector dot product is negative, set to 0
	vec3 diffuse = max (dot(norm, lightDir), 0.0) * lightColor;
	
	//Specular light component calculation
	vec3 viewDir = normalize(viewPos - vPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 20) * lightColor;
	
	if (choice == 0)
		fragColor = vec4 (vColor, 1.0);
	else if (choice == 1)
		fragColor = vec4 (ambient * vColor, 1.0);
	else if (choice == 2)
		fragColor = vec4 (diffuse * vColor, 1.0);
	else if (choice == 3)
		fragColor = vec4 (specular * vColor, 1.0);
	else if (choice == 4)
		fragColor = vec4 ((ambient + diffuse) * vColor, 1.0);
	else
		fragColor = vec4 ((ambient + diffuse + specular) * vColor, 1.0);

} 