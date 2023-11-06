#include <Radiant/Rendering/SceneRendering.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>

#include <Radiant/Rendering/Material.hpp>

namespace Radiant
{
	struct CompositeData
	{
		Memory::Shared<Shader> CompositeShader;
		Memory::Shared<RenderingPass> CompositePass;
		Memory::Shared<Pipeline> CompositePipeline;
		Memory::Shared<class Material> CompositeMaterial;
	};

	struct GeometryData
	{
		Memory::Shared<Shader> GeometryShader;
		Memory::Shared<RenderingPass> GeometryPass;
		Memory::Shared<Pipeline> GeometryPipeline;
	};

	struct SceneInfo
	{
		CompositeData CompositeInfo;
		GeometryData GeometryInfo;

		Memory::Shared<Shader> QuadShader;
		Memory::Shared<Shader> StaticShader;
		std::vector<DrawProperties> MeshDrawList;
		std::vector<Memory::Shared<Mesh>> MeshDrawListWithShader;
		Memory::Shared<TextureCube> EnvRadiance;
		Memory::Shared<TextureCube> EnvIrradiance;
		Memory::Shared<class Material> QuadMaterial;

		Memory::Shared<Texture2D> BRDFLUTTexture;

		uint32_t Samples;

		struct
		{
			glm::mat4 ViewProjection;
			glm::vec3 CameraPosition;
		} Camera;
	};

	static SceneInfo *s_SceneInfo = nullptr;

	SceneRendering::SceneRendering(const Memory::Shared<Scene>& scene)
		: m_Context(scene)
	{
		Init();
	}

	SceneRendering::~SceneRendering()
	{
		delete s_SceneInfo;
	}

	void SceneRendering::Init()
	{
		s_SceneInfo = new SceneInfo();
		s_SceneInfo->QuadShader = Rendering::GetShaderLibrary()->Get("Quad.rads");
		s_SceneInfo->StaticShader = Rendering::GetShaderLibrary()->Get("Static_Shader.rads");
		s_SceneInfo->QuadMaterial = Material::Create(s_SceneInfo->QuadShader);
		/* Composite Pass */

		{
			FramebufferSpecification FrameBufferComposite;
			FrameBufferComposite.ClearColor = glm::vec4(0.5f);
			FrameBufferComposite.Format = ImageFormat::RGBA8;

			RenderingPassSpecification PassComposite;
			PassComposite.TargetFramebuffer = Framebuffer::Create(FrameBufferComposite);
			s_SceneInfo->CompositeInfo.CompositePass = RenderingPass::Create(PassComposite);

			s_SceneInfo->CompositeInfo.CompositeShader = Shader::Create("Resources/Shaders/Scene.rads");
			s_SceneInfo->CompositeInfo.CompositeMaterial = Material::Create(s_SceneInfo->CompositeInfo.CompositeShader);
		}

		/* Geometry Pass */

		{
			FramebufferSpecification FrameBufferGeometry;
			FrameBufferGeometry.ClearColor = glm::vec4(0.5f); 
			FrameBufferGeometry.Format = ImageFormat::RGBA16F;
			s_SceneInfo->Samples = FrameBufferGeometry.Samples = 16;

			RenderingPassSpecification PassGeometry;
			PassGeometry.TargetFramebuffer = Framebuffer::Create(FrameBufferGeometry);
			s_SceneInfo->GeometryInfo.GeometryPass = RenderingPass::Create(PassGeometry);
		}

		s_SceneInfo->BRDFLUTTexture = Texture2D::Create("Resources/Textures/BRDF_LUT.tga");

	}

	void SceneRendering::SubmitScene(Camera* camera)
	{
		if(camera)
			UpdateCamera(camera);
		Flush();
	}

	void SceneRendering::Flush()
	{
		GeometryPass();
		CompositePass();

		s_SceneInfo->MeshDrawList.clear();
		s_SceneInfo->MeshDrawListWithShader.clear();
	}

	void SceneRendering::GeometryPass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->GeometryInfo.GeometryPass);
		{
			if(s_SceneInfo->EnvRadiance)
				UpdateSkyLight();

			for (auto& dc : s_SceneInfo->MeshDrawList)
			{
				Memory::Shared<Material>& material = dc.Material;

				material->SetValue("u_Transform", dc.Transform, UniformTarget::Vertex);
				material->SetValue("u_ViewProjection", s_SceneInfo->Camera.ViewProjection, UniformTarget::Vertex);

				material->SetValue("u_CameraPosition", s_SceneInfo->Camera.CameraPosition, UniformTarget::Fragment);
				material->SetValue("u_BRDFLUTTexture", s_SceneInfo->BRDFLUTTexture);

				if(s_SceneInfo->EnvRadiance)
					material->SetValue("u_EnvRadianceTex", s_SceneInfo->EnvRadiance);
				if (s_SceneInfo->EnvIrradiance)
					material->SetValue("u_EnvIrradianceTex", s_SceneInfo->EnvIrradiance);

				UpdateDirectionalLight(material);
				Rendering::DrawMesh(dc.Mesh, material);
			}
		}
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::CompositePass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->CompositeInfo.CompositePass);
		{
			s_SceneInfo->CompositeInfo.CompositeMaterial->SetValue("u_Exposure", m_Context->m_Exposure, UniformTarget::Fragment);
			s_SceneInfo->CompositeInfo.CompositeMaterial->SetValue("u_TextureSamples", (uint32_t)s_SceneInfo->Samples, UniformTarget::Fragment);
			s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->BindTexture();

		}
		Rendering::DrawFullscreenQuad(s_SceneInfo->CompositeInfo.CompositeMaterial);
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::AddMeshToDrawList(DrawProperties component) const
	{
		s_SceneInfo->MeshDrawList.push_back(component);
	}

	void SceneRendering::AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh) const
	{
		s_SceneInfo->MeshDrawListWithShader.push_back(mesh);
	}

	void SceneRendering::SetSceneVeiwPortSize(const glm::vec2& size)
	{
		if (m_ViewportWidth != size.x || m_ViewportHeight != size.y)
		{
			m_ViewportWidth = size.x;
			m_ViewportHeight = size.y;

			s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);
			s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);


			if (m_Context->ContainsEntityInScene(ComponentType::Camera))
			{
				auto& camera = m_Context->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
				camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), size.x, size.y, 0.1f, 10000.0f));
			}

		}
	}

	void SceneRendering::SetEnvironment(const Environment& environment) const
	{
		SetEnvRadiance(environment.RadianceMap);
		SetEnvIrradiance(environment.IrradianceMap);
	}

	void SceneRendering::SetEnvRadiance(const Memory::Shared<TextureCube>& envRadiance) const
	{
		s_SceneInfo->EnvRadiance = envRadiance;
	}

	void SceneRendering::SetEnvIrradiance(const Memory::Shared<TextureCube>& envIrradiance) const
	{
		s_SceneInfo->EnvIrradiance = envIrradiance;
	}

	Memory::Shared<Image2D> SceneRendering::GetFinalPassImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetImage();
	}

	Memory::Shared<Image2D> SceneRendering::GetFinalDepthImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetDepthImage();
	}

	void SceneRendering::UpdateSkyLight() // TODO(Danya): Fix crush(Done)
	{
		s_SceneInfo->QuadShader->Bind();
		Rendering::DrawFullscreenQuad(s_SceneInfo->QuadMaterial);
		s_SceneInfo->EnvRadiance->Bind();
		s_SceneInfo->QuadMaterial->SetValue("u_InverseVP", glm::inverse(s_SceneInfo->Camera.ViewProjection), UniformTarget::Vertex);
		Rendering::DrawIndexed(6);
	}

	void SceneRendering::UpdateDirectionalLight(Memory::Shared<Material>& material)
	{
		if (m_Context->ContainsEntityInScene(ComponentType::DirectionLight))
		{
			auto dirLight = m_Context->GetEntityByComponentType(ComponentType::DirectionLight)->GetComponent(ComponentType::DirectionLight).As<DirectionLightComponent>()->DirLight;
			material->SetValue("u_LightUniform.Direction", dirLight.Direction, UniformTarget::Fragment);
			material->SetValue("u_LightUniform.Radiance", dirLight.Radiance, UniformTarget::Fragment);
		}
	}

	void SceneRendering::UpdateCamera(Camera* camera)
	{
		s_SceneInfo->Camera.ViewProjection = camera->GetViewProjectionMatrix();
		s_SceneInfo->Camera.CameraPosition = camera->GetPosition();

		camera->Update();
	}

	uint32_t SceneRendering::GetSamplesCount()
	{
		return s_SceneInfo->Samples;
	}

	void SceneRendering::SetSampelsCount(uint32_t count)
	{
		s_SceneInfo->Samples = count;
	}

	std::pair<Memory::Shared<TextureCube>, Memory::Shared<TextureCube>> SceneRendering::CreateEnvironmentMap(const std::string& filepath)
	{
		static Memory::Shared<Shader> equirectangularConversionShader, envFilteringShader, envIrradianceShader;
		const uint32_t cubemapSize = 2048;
		const uint32_t irradianceMapSize = 32;

		Memory::Shared<TextureCube> envUnfiltered = TextureCube::Create(ImageFormat::RGBA16F, cubemapSize, cubemapSize);
		if (!equirectangularConversionShader)
			equirectangularConversionShader = Shader::Create("Resources/Shaders/EquirectangularToCubeMap.glsl");
		Memory::Shared<Texture2D> envEquirect = Texture2D::Create(filepath);
		RADIANT_VERIFY(envEquirect->GetImageFormat() == ImageFormat::RGBA16F, "Texture is not HDR!");

		equirectangularConversionShader->Bind();
		envEquirect->Bind();
		Rendering::SubmitCommand([envUnfiltered, cubemapSize, envEquirect]()
			{
				glBindImageTexture(0, envUnfiltered->GetImage()->GetImageID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
				glGenerateTextureMipmap(envUnfiltered->GetImage()->GetImageID());
			});


		if (!envFilteringShader)
			envFilteringShader = Shader::Create("Resources/Shaders/EnvironmentMipFilter.glsl");

		Memory::Shared<TextureCube> envFiltered = TextureCube::Create(ImageFormat::RGBA16F, cubemapSize, cubemapSize);

		Rendering::SubmitCommand([envUnfiltered, envFiltered]()
			{
				glCopyImageSubData(envUnfiltered->GetImage()->GetImageID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				envFiltered->GetImage()->GetImageID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
				envFiltered->GetWidth(), envFiltered->GetHeight(), 6);
			});

		envFilteringShader->Bind();
		envUnfiltered->Bind();

		Rendering::SubmitCommand([envUnfiltered, envFiltered, cubemapSize]() {
			const float deltaRoughness = 1.0f / glm::max((float)(envFiltered->GetMipLevelCount() - 1.0f), 1.0f);
			for (int level = 1, size = cubemapSize / 2; level < envFiltered->GetMipLevelCount(); level++, size /= 2) // <= ?
			{
				const GLuint numGroups = glm::max(1, size / 32);
				glBindImageTexture(0, envFiltered->GetImage()->GetImageID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glProgramUniform1f(envFilteringShader->GetRendererID(), 0, level * deltaRoughness);
				glDispatchCompute(numGroups, numGroups, 6);
			}
			});

		if (!envIrradianceShader)
			envIrradianceShader = Shader::Create("Resources/Shaders/EnvironmentIrradiance.glsl");

		Memory::Shared<TextureCube> irradianceMap = TextureCube::Create(ImageFormat::RGBA16F, irradianceMapSize, irradianceMapSize);
		envIrradianceShader->Bind();
		envFiltered->Bind();
		Rendering::SubmitCommand([irradianceMap]()
			{
				glBindImageTexture(0, irradianceMap->GetImage()->GetImageID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
				glDispatchCompute(irradianceMap->GetWidth() / 32, irradianceMap->GetHeight() / 32, 6);
			});

		return { envFiltered, irradianceMap };
	}

}