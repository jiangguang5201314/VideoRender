/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


texture Tex0 ;
texture Tex1 ;
texture Tex2 ;

sampler2D YTextue = 
sampler_state
{
	Texture = <Tex0> ;
	MipFilter = LINEAR ;
	MinFilter = LINEAR ;
	MagFilter = LINEAR ;

	AddressU = CLAMP ;
	AddressV = CLAMP ;
};

sampler2D UTextue =
sampler_state
{
	Texture = <Tex1> ;
	MipFilter = LINEAR ;
	MinFilter = LINEAR ;
	MagFilter = LINEAR ;

	AddressU = CLAMP ;
	AddressV = CLAMP ;
};

sampler2D VTextue =
sampler_state
{
	Texture = <Tex2> ;
	MipFilter = LINEAR ;
	MinFilter = LINEAR ;
	MagFilter = LINEAR ;

	AddressU = CLAMP ;
	AddressV = CLAMP ;
};

struct PS_INPUT
{
	float2 uvCoords0 : TEXCOORD0 ;
	float2 uvCoords1 : TEXCOORD1 ;
	float2 uvCoords2 : TEXCOORD2 ;
};

float transparent = 1.0 ;

static float4x4 matYUV2RGB = {	1.164383,  0.0,   1.596027, -0.874202,
								1.164383,  -0.391762, -0.812968, 0.531668,
								1.164383,  2.017232,  0.0, -1.085631,
								0.0, 0.0, 0.0, transparent } ;


float4 main( PS_INPUT input ) : COLOR0
{
	float4 yuvColor ;
	yuvColor.x = tex2D( YTextue, input.uvCoords0 ).x ;
	yuvColor.y = tex2D( UTextue, input.uvCoords1 ).x ;
	yuvColor.z = tex2D( VTextue, input.uvCoords2 ).x ;
	yuvColor.w = 1.0 ;

	return mul( matYUV2RGB, yuvColor ) ;
}