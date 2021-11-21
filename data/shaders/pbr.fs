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

uniform int u_output;

uniform float u_istexture;
uniform sampler2D u_texture;
uniform vec4 u_color;
uniform vec4 u_colorfactor;

uniform float u_ismetal;
uniform sampler2D u_metalness;
uniform float u_metalval;

uniform float u_isrough;
uniform sampler2D u_roughness;
uniform float u_roughval;


vec3 gamma(vec3 c){
return pow(c,vec3(1.0/2.2));}

vec3 degamma(vec3 c){
return pow(c,vec3(2.2));
}

vec3 readTex(){
	vec3 albedo;
	if(u_istexture==1){
		albedo=degamma(texture2D( u_texture, v_uv));}
	else{
		albedo=u_color;}
	return albedo * u_colorfactor;
	}

float readRou(){
	float roughness;
	if(u_isrough==1){
		roughness=texture2D( u_roughness, v_uv).g;}
	else{
		roughness=1;}
	return roughness * u_roughval;
}

float readMet(){
//CASO CONCRETO PARA EL CASCO
	float metalness;
	if(u_ismetal==1){
		metalness= texture2D( u_metalness, v_uv).g;}
	else{
		metalness=1;}
	return metalness * u_metalval;
}




vec3 toneMap(vec3 color)
{
    return color / (color + vec3(1.0));
}



vec3 fresnelRef(float NdL,vec3 f0){
	return(f0+(vec3(1.0)-f0)*pow(1-NdL,5.0));
	}

float normalDist(float NdH,float roughness){
	float alpha = pow(roughness,4);
	float den = pow(NdH,2) * (alpha - 1.0) +1.0;
	return alpha/(PI*pow(den,2));
	//return 1.0;
}

float G1(float prod,float k){
	return prod/(prod*(1-k) + k);
}

float MascShadow(float NdL,float NdV,float roughness){
	float k = pow(1+roughness,2.0)/8.0;
	return G1(NdL,k)*G1(NdV,k);
}

vec3 brdfd(vec3 cdiff){
	return cdiff*RECIPROCAL_PI;
}

vec3 brdfs(float NdL,float NdV,float NdH,float roughness, vec3 f0){
	vec3 F = fresnelRef(NdL,f0);
	float G = MascShadow(NdL,NdV,roughness);
	float D = normalDist(NdH,roughness);
	
	return (F*G*D)/(4.0*NdL*NdV+1e-6);
	}

vec3 brdf(float NdL,float NdV,float NdH,float roughness, vec3 cdiff,vec3 f0){
	return brdfd(cdiff)+brdfs(NdL,NdV,NdH,roughness,f0); 
}


vec3 getPixelColor(vec3 cdiff,vec3 f0,float roughness,vec3 r,vec3 N,float NdL,float NdV,float NdH){


	//Direct Lighting
	vec3 Li = u_lightColor;
	vec3 direct = brdf(NdL,NdV,NdH,roughness,cdiff,f0) * Li*NdL;

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
	float metalness = u_metalval;
	float roughness = u_roughval;
	vec3 albedo = u_colorfactor;
	vec3 cdiff = mix(albedo,vec3(0.0),metalness);
	vec3 f0 = mix(vec3(0.04),albedo,metalness);

	
	vec3 color = getPixelColor(cdiff,f0,roughness,r,N,NdL,NdV,NdH);
	color.rgb = toneMap(color.rgb);



	gl_FragColor = vec4(gamma(color) ,1.0);

}