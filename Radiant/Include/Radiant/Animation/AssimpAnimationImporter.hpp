#pragma once

#include <assimp/scene.h>

#include <Radiant/Animation/Skeleton.hpp>

namespace Radiant::AssimpAnimationImporter
{
	Skeleton ImportSkeleton(const aiScene* scene);
}