#pragma once

#include <glm/glm.hpp>
#include <Radiant/Rendering/Mesh.hpp>

namespace Radiant
{
	class Entity;

	enum class ComponentType
	{
		None = 0, Transform, Mesh
	};

	struct Component : public Memory::RefCounted
	{
	protected:
		virtual const ComponentType GetType() const = 0;

		friend Entity;
	};

	struct TransformComponent : public Component
	{
		TransformComponent() = default;

		TransformComponent(glm::vec3 Position)
			: Position(Position)
		{

		}

		glm::vec3 Position;

	protected:
		virtual const ComponentType GetType() const override { return ComponentType::Transform; }
	};

	struct MeshComponent : public Component
	{
		Memory::Shared<Radiant::Mesh> Mesh;

	protected:
		virtual const ComponentType GetType() const override { return  ComponentType::Mesh; }

		friend Entity;
	};

	struct MaterialComponent : public Component
	{
	protected:
		virtual const ComponentType GetType() const override { return ComponentType::Mesh;  }

		friend Entity;
	};

	template <typename T, typename... Args>
	Memory::Shared<T> CreateNewComponent(Args&&... args)
	{
		return Memory::Shared<T>::Create(std::forward<Args>(args)...);
	}
}