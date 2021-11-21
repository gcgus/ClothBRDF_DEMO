#define PI 3.14159265359
#define RECIPROCAL_PI 0.3183098861837697

//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;

//here create uniforms for all the data we need here

uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;

uniform vec3 u_camera_position;

uniform vec4 u_color;
uniform vec4 u_colorfactor;

uniform vec4 u_subcolor;
uniform vec4 u_sheen;

uniform float u_roughness;


vec3 gamma(vec3 c){
return pow(c,vec3(1.0/2.2));}

vec3 degamma(vec3 c){
return pow(c,vec3(2.2));
}


vec3 toneMap(vec3 color)
{
    return color / (color + vec3(1.0));
}



vec3 fresnelRef(float NdL,vec3 f0){
	return f0;
	}

float normalDist(float NdH,float roughness){
    float invAlpha  = 1.0 / roughness;
    float cos2h = NdH * NdH;
    float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
    return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * PI);
}

vec3 brdfd(vec3 cdiff){
	return cdiff*RECIPROCAL_PI;
}

vec3 brdfs(float NdL,float NdV,float NdH,float roughness, vec3 f0){
	vec3 F = fresnelRef(NdL,f0);
	float D = normalDist(NdH,roughness);
	
	return (F*D)/(4.0*(NdL+NdV-(NdL*NdV)));
	}

vec3 brdf(float NdL,float NdV,float NdH,float roughness, vec3 cdiff,vec3 f0,vec3 subC){
	vec3 Fd=brdfd(cdiff);
	Fd*=saturate((NdL + 0.5) / 2.25);
	
	//subsurface baratillo
	Fd*= saturate(subC + NdL);
	
	vec3 Fs=brdfs(NdL,NdV,NdH,roughness,f0);
	return Fd+Fs * NdL; 
}


vec3 getPixelColor(vec3 cdiff,vec3 f0,float roughness,vec3 r,vec3 N,float NdL,float NdV,float NdH,vec3 subC){

	//Direct Lighting
	vec3 Li = u_lightColor;
	vec3 direct = brdf(NdL,NdV,NdH,roughness,cdiff,f0,subC) * Li;

	return direct;
	}

void main()
{  
	//Vectors
	vec3 L = normalize( u_lightPosition - v_world_position );
    vec3 N = normalize(v_normal);
    vec3 R = normalize( reflect(-L,N) );
    vec3 V = normalize( u_camera_position - v_world_position );
	vec3 H = normalize( L + V );
	vec3 r = reflect(normalize(v_world_position-u_camera_position),v_normal);
	vec2 uv = v_uv;
	

	//Dot Products
	float NdL = max(0.0,dot(N,L));
	float NdV = max(0.0,dot(N,V));
	float NdH = max(0.0,dot(N,H));

	//Material Properties
	float roughness = u_roughness;
	
	
	vec3 subC = u_subcolor.rgb;
	vec3 cdiff = u_color.rgb;
	vec3 f0 = u_sheen;


	
	vec3 color = getPixelColor(cdiff,f0,roughness,r,N,NdL,NdV,NdH,subC);
	color.rgb = toneMap(color.rgb);



	gl_FragColor = vec4(gamma(color) ,1.0);

}