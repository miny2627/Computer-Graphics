#version 330

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_view;
in vec3 v_worldPos;

uniform sampler2D theTexture;

struct DirectionalLight{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	
	vec3 direction;
};

struct PointLight
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
	
	vec3 position;
	float attenuationFactor;
};

struct Material
{
	//we use Md, Ms as texture color
	float specularIntensity; //Ms = texture color * specular intensity
	float shininess; //sh
};

uniform DirectionalLight directionalLight;
uniform PointLight pointLights;
uniform Material material;

out vec4 fragColor;

vec4 CalcDirectionalLight(vec3 color, float ambientIntensity, 
							float diffuseIntensity, vec3 direction)
{
	vec3 normal = normalize(v_normal);
	vec3 view = normalize(v_view);
	vec3 light = normalize(direction);

	//Diffuse term
	float diffuseFactor = max(dot(normal, light),0.0);
	vec3 srcDiff = color * diffuseIntensity;
	vec3 diff = diffuseFactor * srcDiff;

	//Specular term
	//vec3 rVec = normalize(reflect(direction, v_normal));
	vec3 rVec = 2.0 * normal * dot(normal, light) - light;
	float specularFactor = pow(max(dot(rVec, view),0.0),material.shininess);
	vec3 srcSpec = color;
	float matSpec = material.specularIntensity;
	vec3 spec = specularFactor * srcSpec * matSpec;

	//Ambient term
	vec3 ambi = color * ambientIntensity;
	
	return vec4(diff + spec + ambi, 1.0);
	
}

vec4 CalcPointLights()
{
	vec3 direction = pointLights.position - v_worldPos;
	float distance = length(direction);
	direction = normalize(direction);

	vec4 color = CalcDirectionalLight(pointLights.color, pointLights.ambientIntensity,
									pointLights.diffuseIntensity, direction);
	float attenuation = pointLights.attenuationFactor * distance * distance;

	return (color / attenuation);
}

void main()
{
	vec4 dColor = CalcDirectionalLight(directionalLight.color, directionalLight.ambientIntensity,
										directionalLight.diffuseIntensity, directionalLight.direction);
	dColor += CalcPointLights();
	
	fragColor = texture(theTexture, v_texCoord) * dColor;
} 