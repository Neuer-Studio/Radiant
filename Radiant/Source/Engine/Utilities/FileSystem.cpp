#include <Radiant/Utilities/FileSystem.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace Radiant::Utils
{
	bool FileSystem::CreateDirectory(const std::filesystem::path& directory)
	{
		return fs::create_directory(directory);
	}

	bool FileSystem::CreateDirectory(const std::string& directory)
	{
		return CreateDirectory(fs::path(directory));
	}

	bool FileSystem::Exists(const std::filesystem::path& filepath)
	{
		return fs::exists(filepath);
	}

	bool FileSystem::Exists(const std::string& filepath)
	{
		return fs::exists(fs::path(filepath));
	}

	std::string FileSystem::GetFileName(const std::filesystem::path& filepath)
	{
		return filepath.filename().string();
	}

	std::string FileSystem::GetFileName(const std::string& filepath)
	{
		return GetFileName(std::filesystem::path(filepath).string());
	}

}