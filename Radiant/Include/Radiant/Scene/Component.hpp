#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}

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