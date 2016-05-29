#pragma once
#include "Camera.h"
#include "Drawable.h"
#include "GraphicsCore.h"

class Triangle
{
public:
	Triangle(HINSTANCE hInstance, HWND window) : camera(glm::vec2(1280, 720), -2.5f, 60.0f, .1f, 256.0f)
	{
		graphics = std::make_unique<GraphicsCore>(hInstance, window, "triangle", 1280, 720);
	}

	void begin()
	{
		drawable.push_back(Drawable(*graphics->device, *graphics->physicalDevice, graphics->renderPass, graphics->pipelineCache, graphics->frameBuffers, *graphics->swapchain, graphics->cmdPool, *graphics->queue, camera));
	}

	void draw()
	{
		for (auto it = drawable.begin(); it != drawable.end(); ++it)
			(*it).updateUniformBuffers(camera, *graphics->device);

		graphics->render(drawable);
	}

	void end()
	{
	}

	float frameTimer = 1.0f;
	float timerSpeed = 0.25f;
	float timer      = 0.0f;
	bool paused      = false;

	Camera camera;
	float rotationSpeed = 0.3f;

private:
	std::unique_ptr<GraphicsCore> graphics;

	std::vector<Drawable> drawable;
};
