#pragma once
#include "Scene.h"
class Menu : public Scene {
	GLuint fontTextureID;

public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};