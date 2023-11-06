#pragma once

#include <Radiant/Rendering/Texture.hpp>

namespace Radiant {

	class Environment : public Memory::RefCounted
	{
	public:
		Memory::Shared<TextureCube> RadianceMap;
		Memory::Shared<TextureCube> IrradianceMap;

		Environment() = default;
		Environment(const Memory::Shared<TextureCube>& radianceMap, const Memory::Shared<TextureCube>& irradianceMap)
			: RadianceMap(radianceMap), IrradianceMap(irradianceMap) {}
	};


}
