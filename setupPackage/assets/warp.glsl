out vec4 oColor;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec3 col = texture2D(iChannel0, uv).xyz;
	//vec4 col = texture2D(iChannel0, uv);
	//oColor = iAlpha * vec4( col.r, col.g, col.b, 1.0  );

	oColor = iAlpha * vec4( col, 1.0 );
}