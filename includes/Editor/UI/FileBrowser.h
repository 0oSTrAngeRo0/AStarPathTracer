#pragma once

#include <string>
#include <filesystem>

class FileBrowser {
private:
	struct DirectoryNode {
		std::string full_path;
		std::string file_name;
		std::vector<DirectoryNode> children;
		bool is_directory;
	};
	DirectoryNode root;

	void RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
	DirectoryNode CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath);
	void RecursivelyDisplayDirectoryNode(const DirectoryNode& parentNode);
public:
	FileBrowser(const std::string path);
	void OnDrawUi();
};
