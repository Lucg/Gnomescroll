

//uniform sampler2D color_texture;

varying float intensity;

uniform sampler2DArray base_texture;
varying vec3 texCoord;

void main() 
{
	//gl_FragColor = texture2DArray(base_texture, texCoord.xyz);

	//float gamma = 2.2f;

	vec3 color = intensity*(texture2DArray(base_texture, texCoord.xyz).rgb);
	gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );
	gl_FragColor.a = 1.0;
}

/*
	//gl_FragColor = texture2D(color_texture, gl_TexCoord[0].st);

	//vec4 color2 = texture2DArray(textureName,texCoord.xyz).rgb;

	//vec3 color = intensity*texture2DArray(textureName,texCoord.xyz).rgb;
	//vec3 color = intensity*texture2DArray(textureName, vec(tex2.st,1) ).rgb;

	//gl_FragColor = texture2DArray(textureName, vec3( VaryingTexCoord0.x, VaryingTexCoord0.y, 0 ));
	gl_FragColor = texture2DArray(textureName, vec3( VaryingTexCoord0.x, VaryingTexCoord0.y, 0.0 ));

	float gamma = 2.2f;

	vec3 color = intensity*texture2DArray(textureName, vec3( tex2.x, tex2.y, 0 )).rgb;
	gl_FragColor.rgb = pow(color, vec3(1.0 / gamma) );
	gl_FragColor.a = 1.0;
*/

/*
!!!
X-Plane's physical shader is based on conditional compilation - that is, for any given state vector of "tricks" we want to use, we recompile the shader  with some #defines at the front which turn features on and off. The result is a large number of shaders, none of which need conditional logic in-shader. Fill rate isn't consumed by features we don't use.
*/

/*
GLSL Built-In Varyings:

gl_FrontColor				4D vector representing the primitives front color
gl_BackColor				4D vector representing the primitives back color
gl_TexCoord[X]				4D vector representing the Xth texture coordinate
There are some other built-in varyings. See reference [2], page 44 for a full list.

And last but not least there are some built-in types which are used for shader output:

gl_Position				4D vector representing the final processed vertex position. Only 
					available in vertex shader.
gl_FragColor				4D vector representing the final color which is written in the frame 
					buffer. Only available in fragment shader.
gl_FragDepth				float representing the depth which is written in the depth buffer.
					Only available in fragment shader.

*/
