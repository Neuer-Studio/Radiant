#pragma once

namespace Radiant::Memory
{
	template <typename T>
	class Unique
	{
	public:
		Unique(const Unique&) = delete;
		Unique& operator=(const Unique&) = delete;

		Unique(std::nullptr_t)
			: m_Instance(nullptr){}
		Unique() noexcept
			: m_Instance(nullptr){}

		Unique& operator=(Unique&& other) noexcept
		{
			m_Instance = std::exchange(other.m_Instance, nullptr); // NOTE(Danya): Set the other.m_Instance to nullptr and return ex. value (other.m_Instance) to the new instance m_Instance 
																	//(now: m_Instance = other.m_Instance; other.m_Instance = nullptr)
			return *this;
		}

		template<typename U>
		Unique& operator=(Unique<U>&& other) noexcept
		{
			m_Instance = static_cast<T*>(std::exchange(other.m_Instance, nullptr));
			return *this;
		}

		Unique(Unique&& other) noexcept
			: m_Instance(std::exchange(other.m_Instance, nullptr))
		{
		}

		Unique& operator=(T* instance)
		{
			auto* old = std::exchange(m_Instance, instance);

			if (old)
			{
				delete old;
				old = nullptr;
			}

			return *this;
		}

		~Unique() noexcept
		{
			if (m_Instance)
			{
				delete old;
				old = nullptr;
			}
		}
	public:
		T* Get() { return m_Instance; }
		const T* Get() { return m_Instance; }

		void Release()
		{
			if (m_Instance)
			{
				delete m_Instance;
				m_Instance = nullptr;
			}
		}

		operator T& () const& { return *m_Instance; }
		T* operator->() const { return m_Instance; }
	public:
		template<typename... TArgs>
		static Unique<T> Create(TArgs&&... args)
		{
			return Unique<T>(new T(std::forward<TArgs>(args)...));
		}

	private:
		T* m_Instance = nullptr;
	};
}
