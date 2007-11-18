extern matrix view_proj_mat;
extern matrix world_mat;

texture tex;

#define MAX_LIGHTS 5
int maxLights = MAX_LIGHTS;

vector lightPos[MAX_LIGHTS];
float lightRange[MAX_LIGHTS];
vector lightColor[MAX_LIGHTS];
int numLights;
vector cameraPos;
float depthBias;

sampler S0 = sampler_state
{
	Texture = (tex);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

texture shadowMap0;
texture shadowMap1;
texture shadowMap2;
texture shadowMap3;
texture shadowMap4;
sampler ShadowSamplers[MAX_LIGHTS] = {
	sampler_state
	{
		Texture = (shadowMap0);
	},
	sampler_state
	{
		Texture = (shadowMap1);
	},
	sampler_state
	{
		Texture = (shadowMap2);
	},
	sampler_state
	{
		Texture = (shadowMap3);
	},
	sampler_state
	{
		Texture = (shadowMap4);
	}
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
	float3 normal : TEXCOORD4;
};
struct VS_SHADOW_INPUT
{
	vector position : POSITION;
};
struct VS_SHADOW_OUTPUT
{
	vector position : POSITION;
	float3 pos : TEXCOORD0;
};
struct PS_OUTPUT
{
	vector color : COLOR0;
};

VS_OUTPUT Main(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 worldPos = mul(input.position, world_mat);
	output.position = mul(worldPos, view_proj_mat);

	output.texcoords = input.texcoords;

	output.pos = worldPos.xyz;

	output.normal = normalize(mul(input.normal, world_mat));

	return output;
}

PS_OUTPUT PSMain(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;

	const float resolution = 1.0f / 128.0f;
	
	float4 texColor = tex2D(S0, input.texcoords);

	float3 norm = input.normal;

	int j = 0;
	while(j < numLights)
	{
		float3 light0 = normalize(input.pos - lightPos[j]);
		float dist0 = distance(input.pos, lightPos[j]);
		float attenuation0 = 1.0f - (dist0 / lightRange[j]);
		float lambert0 = clamp(dot(norm, -light0), 0.0f, 1.0f);
		dist0 *= depthBias;

		float sample0 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(-resolution, -resolution, -resolution)).r, sample1 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(resolution, -resolution, -resolution)).r, sample2 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(resolution, -resolution, resolution)).r, sample3 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(-resolution, -resolution, resolution)).r, sample4 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(-resolution, resolution, -resolution)).r, sample5 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(resolution, resolution, -resolution)).r, sample6 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(resolution, resolution, resolution)).r, sample7 = dist0 < texCUBE(ShadowSamplers[j], light0 + float3(-resolution, resolution, resolution)).r;
		
		float4 lighting = texColor * attenuation0 * lambert0 * ((sample0 + sample1 + sample2 + sample3 + sample4 + sample5 + sample6 + sample7) / 4);

		output.color += lightColor[j] * lighting;
		j++;
	}
	return output;
}

VS_SHADOW_OUTPUT Shadow(VS_SHADOW_INPUT input) {
	VS_SHADOW_OUTPUT output = (VS_SHADOW_OUTPUT)0;
	float4 worldPos = mul(input.position, world_mat);
	output.position = mul(worldPos, view_proj_mat);

	output.pos = worldPos.xyz;

	return output;
}

PS_OUTPUT ShadowPS(VS_SHADOW_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.color.r = distance(input.pos, cameraPos.xyz);

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
technique Shadow
{
	pass P0
	{
		vertexShader = compile vs_3_0 Shadow();
		pixelShader = compile ps_3_0 ShadowPS();
	}
}