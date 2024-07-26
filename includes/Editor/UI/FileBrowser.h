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
public:
	struct State {
		bool is_changed;
		uint32_t mouse_button;
		bool is_directory;
		std::string full_path;
	};

private:
	std::string root_path;
	DirectoryNode root;
	State current_state;

	void RecursivelyAddDirectoryNodes(DirectoryNode& parentNode, std::filesystem::directory_iterator directoryIterator);
	DirectoryNode CreateDirectryNodeTreeFromPath(const std::filesystem::path& rootPath);
	void RecursivelyDisplayDirectoryNode(const DirectoryNode& parentNode);
	void OnSelected(uint32_t mouse_button, const DirectoryNode& node);
public:
	FileBrowser(const std::string path) : root_path(path){ root = CreateDirectryNodeTreeFromPath(path); }
	void OnDrawUi();
	inline const State& GetCurrentState() const { return current_state; }
	inline void Refresh() { root = CreateDirectryNodeTreeFromPath(root_path); }
};
