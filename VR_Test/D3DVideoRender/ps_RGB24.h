/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

*/


texture Tex0 ;

sampler2D ColorTextue = 
sampler_state
{
	Texture = <Tex0> ;
	MipFilter = POINT ;
	MinFilter = POINT ;
	MagFilter = POINT ;

	AddressU = CLAMP ;
	AddressV = CLAMP ;
};

struct PS_INPUT
{
	float2 uvCoords0 : TEXCOORD0 ;
};

float RGB_dx[4] ;

float4 main( PS_INPUT input ) : COLOR0
{
	float4 rgbColor ;

	rgbColor.r = tex2D( ColorTextue, input.uvCoords0 + float2(RGB_dx[0], 0.0) ).x ;
	rgbColor.g = tex2D( ColorTextue, input.uvCoords0 + float2(RGB_dx[1], 0.0) ).x ;
	rgbColor.b = tex2D( ColorTextue, input.uvCoords0 + float2(RGB_dx[2], 0.0) ).x ;

	rgbColor.a = RGB_dx[3] ;

	return rgbColor ;
}