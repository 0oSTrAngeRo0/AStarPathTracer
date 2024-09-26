#pragma once

#include <string>
#include <filesystem>

class TreeView {
public:
	struct Node {
		std::string id;
		std::string name;
		std::vector<Node> children;
		bool is_leaf;
		Node() = default;
		Node(std::string id, std::string name, std::vector<Node> children, bool is_leaf) :
			id(id), name(name), children(children), is_leaf(is_leaf) {}
	};
	struct State {
		bool is_changed;
		uint32_t mouse_button;
		bool is_leaf;
		std::string id;
	};

	static Node CreateLeaf(std::string id, std::string name) { return Node(id, name, {}, true); }
	static Node CreateNonLeaf(std::string id, std::string name, std::vector<Node> children = {}) { return Node(id, name, children, false); }
	static void DrawUi(const Node& root, State& current_state);
	static void DrawUiNoRoot(const Node& root, State& current_state);
	static Node CreateDirectryNodeTreeFromPath(const std::filesystem::path& path);
private:
	static void RecursivelyAddDirectoryNodes(Node& parent, std::filesystem::directory_iterator directory_iterator);
	static void RecursivelyDisplayDirectoryNode(const Node& parent, State& current_state);
	static void OnSelected(uint32_t mouse_button, const Node& node, State& current_state);
};
