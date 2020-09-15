#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 eyePosition; //camera position(in world space)

out vec2 v_texCoord; //for m_d calculation
out vec3 v_normal; // n vector data(in world space)
out vec3 v_view; // v vector (per vertex)
out vec3 v_worldPos;

void main()
{
	//transform object space normal to world space normal
	v_normal = transpose(inverse(mat3(model))) * norm;
	
	//world tranformed vertex
	v_worldPos = (model * vec4(pos,1.0)).xyz;
	//v vector calculation
	v_view = normalize(eyePosition - v_worldPos);
	
	v_texCoord = tex;
	gl_Position = projection * view * model * vec4(pos, 1.0);
	//same as gl_Position = projection*view*vec4(v_worldPos,1.0);
	
}