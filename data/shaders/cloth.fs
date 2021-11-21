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

float D_Charlie(float roughness, float NoH) {
    // Estevez and Kulla 2017, "Production Friendly Microfacet Sheen BRDF"
    float invAlpha  = 1.0 / roughness;
    float cos2h = NoH * NoH;
    float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
    return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * PI);
}

float V_Neubelt(float NoV, float NoL) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return 1.0 / (4.0 * (NoL + NoV - NoL * NoV));
}

float Fd_Lambert() {
    return 1.0 / PI;
}


vec3 surfaceShading(vec3 H,float NoL,float NoH,float LoH,float NoV,vec3 F,vec3 cdiff,vec3 subc,float roughness) {
    // specular BRDF
	
    float D = D_Charlie(roughness, NoH);
    float V = V_Neubelt(NoV, NoL);
    // Ignore pixel.energyCompensation since we use a different BRDF here
    vec3 Fr = (D * V) * F;

    // diffuse BRDF
    float diffuse = Fd_Lambert();

	//Borrar si no sub
    // Energy conservative wrap diffuse to simulate subsurface scattering
	diffuse *= saturate((NoL + 0.5) / 2.25);

    // We do not multiply the diffuse term by the Fresnel term as discussed in
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    // The effect is fairly subtle and not deemed worth the cost for mobile
    vec3 Fd = diffuse * cdiff;


    // Cheap subsurface scatter
    Fd *= saturate(subc + NoL);
    // We need to apply NoL separately to the specular lobe since we already took
    // it into account in the diffuse lobe
    vec3 color = Fd + Fr * NoL;
    color *= u_lightColor.rgb ;

	//NOSUB
    //vec3 color = Fd + Fr;
    //color *= u_lightColor.rgb;


    return color;
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

	float NoL = max(0.0,dot(N,L));
	float NoH = max(0.0,dot(N,H));
	float LoH = max(0.0,dot(L,H));
	float NoV = max(0.0,dot(N,V));
	
	float metalness = readMet();
	float roughness = readRou();
	vec3 subc = vec3(1);
	vec3 albedo = readTex();
	vec3 cdiff = mix(albedo,vec3(0.0),metalness);
	vec3 f0 = mix(vec3(0.04),albedo,metalness);
	
    gl_FragColor = vec4(surfaceShading(H,NoL,NoH,LoH,NoV,f0,cdiff,subc,roughness),1);
}