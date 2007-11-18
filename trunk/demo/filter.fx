struct VS_INPUT {
	vector pos : POSITION;
	float2 texcoords : TEXCOORD0;
};
struct VS_OUTPUT {
	vector pos : POSITION;
	float2 texcoords : TEXCOORD0;
};
struct PS_OUTPUT {
	vector color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = input.pos;
	output.texcoords = input.texcoords;
	return output;
}

PS_OUTPUT PSMain(VS_OUTPUT input) {
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}

technique Default
{
	pass P0
	{
		vertexShader = compile vs_3_0 VSMain();
		pixelShader = compile ps_3_0 PSMain();
	}
}