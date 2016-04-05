#pragma once

#include "Includes.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Light.h"

class Object
{

protected:

	glm::mat4 matRotation;
	glm::mat4 matModel, matNormal;
	glm::vec3 position, rotation, scale;
	static GLuint defaultTexID;

	Model* model;
	Shader* shader;
	Material material;

	GLuint lineVbo;
	GLuint lineVao;
	std::vector<glm::vec3> lineVertices;
	std::vector<BoundingBox> listBoundingBox;
	bool isBoundingBoxVisible;
	bool isVisible;
	bool isWireframeMode;

	virtual void updateModelMatrix();
	virtual void updateNormalMatrix();

	void makeBoundingBoxData();

public:

	Object(glm::vec3 position = glm::vec3());
	~Object();

	virtual void load(std::string modelname, std::string shadername = "basic");
	virtual void update(float dt);
	virtual void draw(Light* light = nullptr);
	virtual void draw(Light* light, bool depthMode);

	// === Accessors ===

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);
	void setBoundingBoxVisible(bool isVisible);
	void setVisible(bool isVisible);
	void setWireframeMode(bool isWireframe);

	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	glm::vec3 getScale() const;
	glm::mat4 getRotationMatrix() const;
	Material* getMaterial();
	Shader* getShader() const;
	bool getBoundingBoxVisible() const;
	bool getVisible() const;
	bool getWireframeMode() const;
	const std::vector<BoundingBox>* getBoundingBoxList() const;
	static GLuint getDefaultTex();
};