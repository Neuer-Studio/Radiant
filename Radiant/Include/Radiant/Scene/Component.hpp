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
		virtual ComponentType GetType() = 0;

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

		static ComponentType StaticGetType() { return ComponentType::Transform; }
	protected:
		virtual ComponentType GetType() override { return StaticGetType(); }
	};

	struct MeshComponent : public Component
	{
		Memory::Shared<Radiant::Mesh> Mesh;

		static ComponentType StaticGetType() { return ComponentType::Mesh; }
	protected:
		virtual ComponentType GetType() override { return StaticGetType(); }

		friend Entity;
	};

	template <typename T, typename... Args>
	Memory::Shared<T> CreateNewComponent(Args&&... args)
	{
		return Memory::Shared<T>::Create(std::forward<Args>(args)...);
	}
}