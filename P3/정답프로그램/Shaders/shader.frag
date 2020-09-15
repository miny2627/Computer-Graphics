#version 330

in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos; //from vert

out vec4 color;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform DirectionalLight directionalLight;

uniform sampler2D theTexture;
uniform Material material;

uniform sampler2D directionalShadowMap; //shadow map tex

uniform vec3 eyePosition;

float CalcDirectionalShadowFactor(DirectionalLight light)
{
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w; //NDC coordinate projection result
	projCoords = (projCoords * 0.5) + 0.5; //[-1,1] (NDC) -> [0,1] (Depth)
	
	float current = projCoords.z; //how far the fragment lies
	
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);
	float bias = max(0.05*(1.0-dot(normal,lightDir)),0.005);
	
	//PCF calc
	float shadow = 0.0;
	vec2 texelSize = 1.0/textureSize(directionalShadowMap,0);
	
	for(int x=-1;x<=1;x++)
	{
		for(int y=-1;y<=1;y++)
		{
			float pcfDepth = texture(directionalShadowMap,projCoords.xy + vec2(x,y)*texelSize).r;
			shadow += current-bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambientColor = vec4(light.color,1.0) * light.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)),0.0);
	vec4 diffuseColor = vec4(light.color, 1.0) * light.diffuseIntensity * diffuseFactor;
	
	vec4 specularColor = vec4(0,0,0,0);
	
	if(diffuseFactor > 0.0)
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);
		//use built-in reflect function
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));
		
		float specularFactor = dot(fragToEye, reflectedVertex);
		if(specularFactor > 0.0)
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0);
		}
	}
	
	return (ambientColor + (1.0-shadowFactor) * (diffuseColor + specularColor));
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

void main()
{
	vec4 finalColor = CalcDirectionalLight();
	
	color = texture(theTexture, TexCoord) * finalColor;
}