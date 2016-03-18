/*
{
	"title" : "simplecolor",
	"value" : 1,
	"url" : "http://www.batchass.fr/"
} 
*/
// Bruce, 2015
uniform vec3  iResolution;  // viewport resolution (in pixels)
uniform vec3  iColor;
uniform float iGlobalTime;
out vec4 oColor; 
void main(void)
{
	oColor = vec4(0.15, cos(iGlobalTime), cos(iGlobalTime), 1.0); 
}