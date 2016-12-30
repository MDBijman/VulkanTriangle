#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OrthoCamera
{
public:
	OrthoCamera()
	{
		position = glm::vec3(0.0f, 0.0f, 1.0f);
		rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		matrices.projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);
	}

	// Moves the camera to the right.
	void move(const glm::vec3 movement, const float dt)
	{
		position += movement * dt;
		updateView();
	}

	// Rotates the camera.
	void rotate(const glm::vec3 r, const float dt)
	{
		rotation += r * dt;
		updateView();
	}

	const auto& getMatrices()
	{
		return matrices;
	}

private:
	void updateView()
	{
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);

		glm::mat4 rotationMatrix;
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), { 1.0, 0.0, 0.0 });
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), { 0.0, 1.0, 0.0 });
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), { 0.0, 0.0, 1.0 });

		matrices.view = rotationMatrix * translationMatrix;
	}

	struct
	{
		glm::mat4 view;
		glm::mat4 projection;
	} matrices;
	
	glm::vec3 position;
	glm::vec3 rotation;
};