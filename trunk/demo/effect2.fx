extern matrix view_proj_mat;
extern matrix world_mat;

texture backBuffer;

#define MAX_LIGHTS 5
int maxLights = MAX_LIGHTS;

vector lightPos[MAX_LIGHTS];
float lightRange[MAX_LIGHTS];
vector lightColor[MAX_LIGHTS];
int numLights;
vector filter[4];
vector cameraPos;
float depthBias;

sampler BackBufferSampler = sampler_state
{
	Texture = (backBuffer);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	ADDRESSU = CLAMP;
	ADDRESSV = CLAMP;
};

struct VS_INPUT
{
	vector position : POSITION;
	float2 texcoords : TEXCOORD0;
	float3 normal : NORMAL0;
};
struct VS_OUTPUT
{
	vector position : POSITION;
	float2 texcoords : TEXCOORD0;
	float3 pos : TEXCOORD1;
	float3 screenpos : TEXCOORD2;
	float3 normal : TEXCOORD3;
};
struct PS_OUTPUT
{
	vector color : COLOR0;
};

VS_OUTPUT Main(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 worldPos = mul(input.position, world_mat);
	output.position = mul(worldPos, view_proj_mat);
	
	output.screenpos = output.position.xyz;

	output.texcoords = input.texcoords;

	output.pos = worldPos.xyz;

	output.normal = normalize(mul(input.normal, world_mat));

	return output;
}

PS_OUTPUT PSMain(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	
	float3 samplePos = input.screenpos;
	samplePos.xy /= samplePos.z;
	samplePos.xy += 1.0f;
	samplePos.xy /= 2.0f;
	samplePos.y = 1.0f - samplePos.y;
	samplePos.xy += (input.normal.xy * 0.05f);
	output.color = tex2D(BackBufferSampler, samplePos.xy);


	float3 norm = input.normal;

	int j = 0;
	while(j < numLights)
	{
		float3 light0 = normalize(input.pos - lightPos[j]);
		float dist0 = distance(input.pos, lightPos[j]);
		float attenuation0 = 1.0f - (dist0 / lightRange[j]);
		float specular0 = clamp(pow(dot(reflect(-light0, norm), normalize(cameraPos - input.pos)), 32), 0.0f, 1.0f);
		dist0 *= depthBias;

		float lighting = attenuation0 * specular0;

		output.color += lightColor[j] * lighting;
		j++;
	}
	return output;
}

technique Default
{
	pass P0
	{
		vertexShader = compile vs_3_0 Main();
		pixelShader = compile ps_3_0 PSMain();
	}
}