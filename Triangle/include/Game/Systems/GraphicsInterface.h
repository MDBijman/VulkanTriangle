#pragma once
#include "Modules/ECS/ECS.h"
#include "Game/GameConfig.h"
#include "Game/Components/SpriteShader.h"

namespace Systems
{
	class GraphicsInterface : public MySystem
	{
	public:
		explicit GraphicsInterface(GameGraphics* graphics) : graphics(graphics) {}

		void update(MyECSManager& ecs, double dt) override
		{
			graphics->prepare();

			auto cameras = ecs.filterEntities<Filter<Components::Camera2D>>();
			auto& camera = ecs.getComponent<Components::Camera2D>(cameras.at(0));

			auto entities = ecs.filterEntities<SpriteFilter>();
			for (auto entity : entities)
			{
				//auto& texture = ecs.getComponent<Components::Texture>(entity);
				//auto& mesh = ecs.getComponent<Components::Mesh>(entity);
				auto& buffers = ecs.getComponent<Components::CommandBuffers>(entity);
				auto& pipeline = ecs.getComponent<Components::Pipeline>(entity);
				auto& shader = ecs.getComponent<Components::SpriteShader>(entity);

				graphics->render<Components::ShaderType>(buffers, shader, camera);
			}

			graphics->present();
		}

	private:
		GameGraphics* graphics;
	};
}
