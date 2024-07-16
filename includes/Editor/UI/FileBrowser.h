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
	DirectoryNode current_selection;
	bool is_selection_changed;

	void RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
	DirectoryNode CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath);
	void RecursivelyDisplayDirectoryNode(const DirectoryNode& parentNode);
public:
	FileBrowser(const std::string path);
	void OnDrawUi();
	inline const DirectoryNode& GetCurrentSelection() const { return current_selection; }
	inline const bool IsSelectionChanged() const { return is_selection_changed; }
};
