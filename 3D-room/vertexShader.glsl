#version 330 core
layout (location = 0) in vec3 aPos;		// the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;	// the position variable has attribute position 1
layout (location = 2) in vec3 bPos;		// the position variable has attribute position 2
layout (location = 3) in vec3 bNormal;	// the position variable has attribute position 3

out vec3 vNormal;	//normal vector for the vertices 
out vec3 vPos;		//vertex position for light calculation

uniform int obj;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	if (obj == 1) {
		gl_Position = proj * view * vec4 (aPos, 1.0);
		vNormal = aNormal;
		//vertex position for light effect calculation - should be fixed regardless of camera postion
		//Hence, only multiply with model matrix. But for object 1 (ground), we never perform any transform
		vPos = aPos;
	}

	//Need to transform normal vector & view vector based on model matrix as well as we have changed
	//the object orientation (rotation & scaling). 
	else if (obj == 2) {
		gl_Position = proj * view * model * vec4 (bPos, 1.0);
		vNormal = vec3 ( model * vec4(bNormal, 0.0));
		vPos = vec3 (model * vec4(bPos, 1.0));
	}
}