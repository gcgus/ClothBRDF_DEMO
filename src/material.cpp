#include "material.h"
#include "texture.h"
#include "application.h"
#include "extra/hdre.h"



//Usamos el metodo constructor para asignar un shader y textura al material.
StandardMaterial::StandardMaterial()
{
	color = vec4(0.7, 0.7, 0.7, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
}

StandardMaterial::~StandardMaterial()
{

}

void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);
	shader->setUniform("u_output", Application::instance->output);

	shader->setUniform("u_color", color);
	shader->setUniform("u_exposure", Application::instance->scene_exposure);

	if (texture)
		shader->setUniform("u_texture", texture);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//creamos el material de phong y le pasamos la textura y shaders, tambien inicializamos alpha que es el shininess
PhongMaterial::PhongMaterial()
{
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	alpha = 0.9;
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::setUniforms(Camera* camera, Matrix44 model, light* LightSource, Vector3 ambient)
{
	StandardMaterial::setUniforms(camera, model);

	shader->setUniform("ambientLight", ambient);
	shader->setUniform("diffuseLight", LightSource->color);
	shader->setUniform("specularLight", LightSource->color);
	shader->setUniform("lightPosition", LightSource->pos);

	shader->setUniform("shininess", this->alpha);
}

void PhongMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model, Application::instance->scene_lightsource, Application::instance->scene_ambient);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

SkyboxMaterial::SkyboxMaterial()
{

}

SkyboxMaterial::~SkyboxMaterial()
{
}

void SkyboxMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_center", camera->center);
	shader->setUniform("u_model", model);
	shader->setUniform("u_cube", texture);

}

void SkyboxMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void SkyboxMaterial::renderInMenu()
{
}

ReflectiveMaterial::ReflectiveMaterial()
{
	this->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/reflective.fs");

}

ReflectiveMaterial::~ReflectiveMaterial()
{
}

void ReflectiveMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_eye", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_cube", Application::instance->skybox->material->texture);
}

void ReflectiveMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (shader && mesh)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void ReflectiveMaterial::renderInMenu()
{
}

PBRMaterial::PBRMaterial()
{
	if (model == 0) {

	}
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/pbr.fs");
	this->metalness = NULL;
	this->roughness = NULL;
	this->normalmap = NULL;
	this->emissionmap = NULL;
	this->aomap = NULL;
	metalval = 1;
	roughval = 1;
	color_factor = vec4(1., 1., 1., 1.f);
	color = vec4(0.7, 0.7, 0.7, 1.f);
}

PBRMaterial::~PBRMaterial()
{
}

void PBRMaterial::setUniforms(Camera* camera, Matrix44 model, light* LightSource)
{
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", color);
	shader->setUniform("u_lightPosition", LightSource->pos);
	shader->setUniform("u_lightColor", LightSource->color);
	shader->setUniform("u_metalval", metalval);
	shader->setUniform("u_roughval", roughval);
	shader->setUniform("u_colorfactor", color_factor);
	shader->setUniform1("u_output", out);

	if (texture) {
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_istexture", 1.f);

	}
	else {
		shader->setUniform("u_istexture", 0.f);
	}

	if (metalness) {
		shader->setUniform("u_metalness", metalness, 1);
		shader->setUniform("u_ismetal", 1.f);
		

	}
	else {
		shader->setUniform("u_ismetal", 0.f);
	}

	if (roughness) {
		shader->setUniform("u_roughness", roughness, 2);
		shader->setUniform("u_isrough", 1.f);
		
	}
	else {
		shader->setUniform("u_isrough", 0.f);
	}

	if (normalmap) {
		shader->setUniform("u_nmap", normalmap, 3);
		shader->setUniform("u_isnmap", 1.f);
	}
	else {
		shader->setUniform("u_isnmap", 0.f);
	}
	if (emissionmap) {
		shader->setUniform("u_emap", emissionmap, 4);
		shader->setUniform("u_isemap", 1.f);
	}
	else {
		shader->setUniform("u_isemap", 0.f);
	}
	if (aomap) {
		shader->setUniform("u_aomap", aomap, 5);
		shader->setUniform("u_isaomap", 1.f);
	}
	else {
		shader->setUniform("u_isaomap", 0.f);
	}

}

void PBRMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model, Application::instance->scene_lightsource);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void PBRMaterial::renderInMenu()
{
		ImGui::DragFloat("Metalness Factor", (float*)&metalval, 0.01f, 0.f, 1);
		ImGui::DragFloat("Roughness Factor", (float*)&roughval, 0.01f, 0.f, 1);
		ImGui::ColorEdit3("Color Factor", (float*)&color_factor);


		if (ImGui::TreeNode("Output")) {
			
			bool changed = false;
			changed |= ImGui::Combo("Output", (int*)&outo, "COMPLETE\0ALBEDO\0ROUGHNESS\0\METALNESS\0NORMALS\0");
			if (changed) {
				out = outo;
			}
			ImGui::TreePop();
		}
}

VolumeMaterial::VolumeMaterial(){
shader = Shader::Get("data/shaders/basic.vs", "data/shaders/volume.fs");
vol = new Volume();
vol->loadPNG("data/volumes/foot_16_16.png");
texture = new Texture();
texture->create3DFromVolume(vol,GL_CLAMP_TO_EDGE);
}


VolumeMaterial::~VolumeMaterial()
{
}

void VolumeMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_local", model.inverse()*camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_volume", texture);
}

void VolumeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

ClothMaterial::ClothMaterial()
{
	if (model == 0) {

	}
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cloth2.fs");
	metalval = 1;
	roughval = 1;
	color_factor = vec4(1., 1., 1., 1.f);
	color = vec4(0.7, 0.7, 0.7, 1.f);
	subsur = vec4(0.7, 0.7, 0.7, 1.f);
	sheen = vec4(0.7, 0.7, 0.7, 1.f);

}

ClothMaterial::~ClothMaterial()
{
}

void ClothMaterial::setUniforms(Camera* camera, Matrix44 model, light* LightSource)
{
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", color);
	shader->setUniform("u_sheen", sheen);
	shader->setUniform("u_subcolor", subsur);

	shader->setUniform("u_lightPosition", LightSource->pos);
	shader->setUniform("u_lightColor", LightSource->color);

	shader->setUniform("u_roughness", roughval);
	shader->setUniform("u_colorfactor", color_factor);
	shader->setUniform1("u_output", out);

}

void ClothMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model, Application::instance->scene_lightsource);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void ClothMaterial::renderInMenu()
{
	ImGui::DragFloat("Roughness", (float*)&roughval, 0.01f, 0.f, 1);
	ImGui::ColorEdit3("Base Color", (float*)&color);
	ImGui::ColorEdit3("Sheen Color", (float*)&sheen);
	ImGui::ColorEdit3("SubSurface Color", (float*)&subsur);


}
