#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "extra/hdre.h"
#include "light.h"
#include "volume.h"

class Material {
public:

	Shader* shader = NULL;
	Texture* texture = NULL;
	vec4 color;

	virtual void setUniforms(Camera* camera, Matrix44 model) = 0;
	virtual void render(Mesh* mesh, Matrix44 model, Camera* camera) = 0;
	virtual void renderInMenu() = 0;
};

class StandardMaterial : public Material {
public:

	StandardMaterial();
	~StandardMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();
};

class WireframeMaterial : public StandardMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Matrix44 model, Camera* camera);
};

class PhongMaterial : public StandardMaterial {
public:
	float alpha;

	PhongMaterial();
	~PhongMaterial();

	void setUniforms(Camera* camera, Matrix44 model, light* LightSource, Vector3 ambient);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
};

class PBRMaterial : public StandardMaterial {
public:
	Texture* roughness = NULL;
	Texture* metalness = NULL;
	Texture* normalmap = NULL;
	Texture* emissionmap = NULL;
	Texture* aomap = NULL;

	float metalval;
	float roughval;
	vec4 color_factor;
	unsigned int outo = 0;
	int out = 0;
	int model = 0;

	PBRMaterial();
	~PBRMaterial();

	void setUniforms(Camera* camera, Matrix44 model, light* LightSource);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();
};

class SkyboxMaterial :public Material {
public:
	SkyboxMaterial();
	~SkyboxMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();
};

class ReflectiveMaterial :public Material {
public:
	ReflectiveMaterial();
	~ReflectiveMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();
};

class VolumeMaterial : public StandardMaterial {
public:
	Volume* vol;

	VolumeMaterial();

	~VolumeMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
};

class ClothMaterial : public StandardMaterial {
public:
	Texture* roughness = NULL;
	

	float metalval;
	float roughval;
	vec4 color_factor;
	unsigned int outo = 0;
	int out = 0;
	int model = 0;
	vec4 sheen;
	vec4 subsur;

	ClothMaterial();
	~ClothMaterial();

	void setUniforms(Camera* camera, Matrix44 model, light* LightSource);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void renderInMenu();
};

#endif