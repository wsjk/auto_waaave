precision highp float;

uniform sampler2D tex0;

uniform float sharpenAmount;
uniform float vSharpenAmount;
varying vec2 texCoordVarying;

//-------------------------
vec3 rgb2hsb(in vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
//---------------------------
vec3 hsb2rgb(in vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
//-------------------------
void main()
{
	vec4 outColor=vec4(1.0,0.0,1.0,1.0);

	// OPTIMIZATION: Reduced texture samples from 8 to 4 diagonal samples
	float X=2.0*.0015625;
	float Y=2.0*.0020833;
	
	// Get original color first
	vec4 ogColor=texture2D(tex0,texCoordVarying);
	vec3 ogColorHSB=rgb2hsb(ogColor.rgb);

	float VVV=ogColorHSB.z;
	float totalSharpen = sharpenAmount + (vSharpenAmount*VVV);

	// OPTIMIZATION: Skip expensive sharpening if amount is near zero
	if(abs(totalSharpen) < 0.001) {
		gl_FragColor = ogColor;
		return;
	}

	// OPTIMIZATION: Sample and convert in one pass, only brightness channel
	float colorSharpenBright =
			rgb2hsb(texture2D(tex0, texCoordVarying+vec2(X,Y)).rgb).z +
			rgb2hsb(texture2D(tex0, texCoordVarying+vec2(-X,Y)).rgb).z +
			rgb2hsb(texture2D(tex0, texCoordVarying+vec2(-X,-Y)).rgb).z +
			rgb2hsb(texture2D(tex0, texCoordVarying+vec2(X,-Y)).rgb).z;

	colorSharpenBright=colorSharpenBright*.25;  // Average of 4 samples

	// Apply sharpening
	ogColorHSB.z-=totalSharpen*colorSharpenBright;

	// Boost brightness and hue when sharpening
	if(totalSharpen>0.0){
		float boost = 1.0 + totalSharpen*.45;
		ogColorHSB.z *= boost;
		ogColorHSB.x *= (1.0 + totalSharpen*.25);
	}
	
	outColor.rgb=hsb2rgb(ogColorHSB);
	outColor.a=1.0;
	gl_FragColor = outColor;
}
