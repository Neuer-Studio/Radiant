#include <Radiant/Utilities/FileSystem.hpp>
#include <Radiant/Platform/Filesystem/Windows/WindowsFileSystem.hpp>

#include <filesystem>

namespace fs = std::filesystem;

namespace Radiant::Utils
{
	class WindowsFileSystem;
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
		return std::filesystem::path(filepath).filename().string();
	}

	std::filesystem::path FileSystem::OpenFileDialog(const char* filter)
	{
		return WindowsFileSystem::OpenFileDialog(filter);
	}

}