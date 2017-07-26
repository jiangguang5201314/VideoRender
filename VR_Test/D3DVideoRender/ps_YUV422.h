/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


texture Tex0 ;
texture Tex1 ;

sampler2D YUVTextue = 
sampler_state
{
	Texture = <Tex0> ;
	MipFilter = POINT ;
	MinFilter = POINT ;
	MagFilter = POINT ;

	AddressU = CLAMP ;
	AddressV = CLAMP ;
};

sampler2D ParityTextue = 
sampler_state
{
	Texture = <Tex1> ;
	MipFilter = POINT ;
	MinFilter = POINT ;
	MagFilter = POINT ;

	AddressU = CLAMP ;
	AddressV = CLAMP ;
};

struct PS_INPUT
{
	float2 uvCoords0 : TEXCOORD0 ;
	float2 uvCoords1 : TEXCOORD1 ;
};

float transparent = 1.0 ;

static float4x4 matYUV2RGB = {	1.164383,  0.0,   1.596027, -0.874202,
								1.164383,  -0.391762, -0.812968, 0.531668,
								1.164383,  2.017232,  0.0, -1.085631,
								0.0, 0.0, 0.0, transparent } ;

static float3 interpolation = { 9.0 / 16.0, -1.0 / 16.0, 1.0 / 510.0} ;

float YUV_dx[9] ;

float4 main( PS_INPUT input ) : COLOR0
{
	float4 yuvColor ;

	yuvColor.x = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[0], 0.0)).x ;

	float parity = tex2D( ParityTextue, input.uvCoords1 ).x ;
	if ( parity > 0.5 ) // odd
	{	
		float3x2 c ;
		c._m00 = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[1], 0.0)).x 
			   + tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[2], 0.0)).x ;

		c._m10 = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[3], 0.0)).x
			   + tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[4], 0.0)).x ;
		c._m20 = 1.0 ;
		
		c._m01 = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[5], 0.0)).x
		      + tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[6], 0.0)).x ;

		c._m11 = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[7], 0.0)).x
			   + tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[8], 0.0)).x ;
		c._m21 = 1.0 ;

		yuvColor.yz = mul ( interpolation, c ) ;
	}
	else
	{
		yuvColor.y = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[5], 0.0)).x ;
		yuvColor.z = tex2D( YUVTextue, input.uvCoords0 + float2(YUV_dx[2], 0.0)).x ;
	}
	yuvColor.w = 1.0 ;

	return mul( matYUV2RGB, yuvColor ) ;
}