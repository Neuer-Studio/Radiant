#pragma once

#include <glm/glm.hpp>

namespace Radiant
{
	class Entity;

	enum class ComponentType
	{
		None = 0, Transform, Mesh
	};

	struct Component
	{
	protected:
		virtual ComponentType GetType() = 0;

		friend Entity;
	};

	struct TransformComponent : public Component
	{
		glm::vec3 Position;
	protected:
		virtual ComponentType GetType() override { return ComponentType::Transform; }
	};

	struct MeshComponent : public Component
	{
		glm::vec3 Position;
	protected:
		virtual ComponentType GetType() override { return ComponentType::Mesh; }
	};
}