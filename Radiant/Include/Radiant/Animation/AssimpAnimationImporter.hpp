#pragma once

#include <assimp/scene.h>

#include <Radiant/Animation/Skeleton.hpp>
#include <Radiant/Animation/Animation.hpp>

namespace Radiant::AssimpAnimationImporter
{
	std::vector<std::string> GetAnimationNames(const aiScene* scene);

	Skeleton ImportSkeleton(const aiScene* scene);
	Animation ImportAnimation(const aiScene* scene, const std::string_view animationName, const Skeleton& skeleton);
}