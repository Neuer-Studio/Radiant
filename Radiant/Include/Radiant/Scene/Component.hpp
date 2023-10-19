#pragma once

#include <glm/glm.hpp>
#include <Radiant/Rendering/Mesh.hpp>
#include <Radiant/Core/Camera.hpp>
#include <Radiant/Rendering/Texture.hpp>

namespace Radiant
{
	class Entity;

	enum class ComponentType
	{
		None = 0, Transform, Mesh, Camera, Cube
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
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

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

	struct CameraComponent : public Component
	{
		Camera Camera;
	protected:
		virtual const ComponentType GetType() const override { return ComponentType::Camera; }

		friend Entity;
	};

	struct CubeComponent : public Component
	{
		Memory::Shared<TextureCube> Cube;
	protected:
		virtual const ComponentType GetType() const override { return ComponentType::Cube; }

		friend Entity;
	};

	template <typename T, typename... Args>
	Memory::Shared<T> CreateNewComponent(Args&&... args)
	{
		return Memory::Shared<T>::Create(std::forward<Args>(args)...);
	}
}