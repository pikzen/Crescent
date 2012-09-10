// Globals
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;
	
	// Multiply the position with the world, ortho and projection matrix
	// for rendering onscreen
	output.position = mul(input.position, worldMatrix); // The world matrix is the identity matrix
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// store the input color for the pixel shader
	output.color = input.color;

	return output;
}