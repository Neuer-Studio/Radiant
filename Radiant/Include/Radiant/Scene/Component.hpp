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
	class Environment;

	enum class ComponentType
	{
		None = 0, Transform, Mesh, Camera, SkyBox, DirectionLight, Material
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

		explicit operator bool() const {
			return Mesh.Ptr() != nullptr;
		}
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

	struct SkyBoxComponent : public Component
	{
		std::string Name;
		std::filesystem::path FilePath;

		Memory::Shared<Environment> Environment;
	protected:
		virtual const ComponentType GetType() const override { return ComponentType::SkyBox; }

		friend Entity;
	}; 
	
	struct MaterialComponent : public Component
	{
		MaterialComponent(const Memory::Shared<Material>& material)
			: Material(material)
		{
		}
		Memory::Shared<Material> Material;
	protected:
		virtual const ComponentType GetType() const override { return ComponentType::Material; }

		friend Entity;
	};

	struct DirectionLightComponent : public Component
	{
		struct
		{
			glm::vec3 Direction = glm::vec3(1.0f);
			glm::vec3 Radiance = glm::vec3(1.0f);
		} DirLight;

	protected:
		virtual const ComponentType GetType() const override { return ComponentType::DirectionLight; }

		friend Entity;
	};

	template <typename T, typename... Args>
	Memory::Shared<T> CreateNewComponent(Args&&... args)
	{
		return Memory::Shared<T>::Create(std::forward<Args>(args)...);
	}
}