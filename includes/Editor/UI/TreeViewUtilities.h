#pragma once

#include <string>
#include <filesystem>

class TreeView {
public:
	struct Node {
		std::string full_path;
		std::string name;
		std::vector<Node> children;
		bool is_not_leaf;
	};
	struct State {
		bool is_changed;
		uint32_t mouse_button;
		bool is_not_leaf;
		std::string full_path;
	};

	static void DrawUi(const Node& root, State& current_state);
	static Node CreateDirectryNodeTreeFromPath(const std::filesystem::path& path);
private:
	static void RecursivelyAddDirectoryNodes(Node& parent, std::filesystem::directory_iterator directory_iterator);
	static void RecursivelyDisplayDirectoryNode(const Node& parent, State& current_state);
	static void OnSelected(uint32_t mouse_button, const Node& node, State& current_state);
};
