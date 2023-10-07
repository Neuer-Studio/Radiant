#pragma once

#include <Radiant/Memory/Buffer.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>

namespace Radiant
{

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Int, Int2, Int3, Int4, Bool
	};

	inline uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:
			case ShaderDataType::Int: return 4;

			case ShaderDataType::Float2:
			case ShaderDataType::Int2: return 4 * 2;

			case ShaderDataType::Float3:
			case ShaderDataType::Int3: return 4 * 3;

			case ShaderDataType::Float4:
			case ShaderDataType::Int4: return 4 * 4;

			case ShaderDataType::Bool: return 1;

		}
	}
	enum class VertexBufferUsage
	{
		None = 0, Static = 1, Dynamic = 2
	};

	struct VertexBufferElement
	{
		ShaderDataType Type;
		std::string Name;
		std::size_t Offset;
		std::size_t Size;
		bool Normalized;

		VertexBufferElement() = default;

		VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Type(type), Name(name), Offset(0), Size(ShaderDataTypeSize(type)), Normalized(normalized)
		{}

		inline uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:   return 1;
				case ShaderDataType::Float2:  return 2;
				case ShaderDataType::Float3:  return 3;
				case ShaderDataType::Float4:  return 4;
				case ShaderDataType::Int:     return 1;
				case ShaderDataType::Int2:    return 2;
				case ShaderDataType::Int3:    return 3;
				case ShaderDataType::Int4:    return 4;
				case ShaderDataType::Bool:    return 1;
			}

			RADIANT_VERIFY(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() {}
		VertexBufferLayout(const std::initializer_list <VertexBufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }

		std::vector<VertexBufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<VertexBufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<VertexBufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			std::size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

		std::vector<VertexBufferElement> m_Elements;
		std::size_t m_Stride = 0;
	};

	class VertexBuffer : public Memory::RefCounted
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void SetData(Memory::Buffer buffer, std::size_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual std::size_t GetCount() const = 0;
		virtual std::size_t GetSize() const = 0;

		static Memory::Shared<VertexBuffer> Create(Memory::Buffer data, VertexBufferUsage usage = VertexBufferUsage::Static);
		static Memory::Shared<VertexBuffer> Create(std::size_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	};
}