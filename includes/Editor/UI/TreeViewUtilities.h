#pragma once

#include <string>
#include <filesystem>

class TreeView {
public:
	using NodeId = std::string;
	struct Node {
		NodeId id;
		std::string name;
		std::vector<Node> children;
		bool is_leaf;
		Node() = default;
		Node(std::string id, std::string name, std::vector<Node> children, bool is_leaf) :
			id(id), name(name), children(children), is_leaf(is_leaf) {}
	};
	struct Result {
		std::uint32_t mouse_button;
		std::optional<std::reference_wrapper<const TreeView::Node>> clicked;

		Result() : mouse_button(0), clicked(std::nullopt) {}
	};

	static Node CreateLeaf(std::string id, std::string name) { return Node(id, name, {}, true); }
	static Node CreateNonLeaf(std::string id, std::string name, std::vector<Node> children = {}) { return Node(id, name, children, false); }
	static Result DrawUi(const Node& root, const NodeId& selected);
	static Result DrawUiNoRoot(const Node& root, const NodeId& selected);
	static Node CreateDirectryNodeTreeFromPath(const std::filesystem::path& path);
private:
	static void RecursivelyAddDirectoryNodes(Node& parent, std::filesystem::directory_iterator directory_iterator);
	static void RecursivelyDisplayDirectoryNode(const Node& parent, Result& result, const NodeId& selected);
	static void OnSelected(uint32_t mouse_button, const Node& node, Result& result);
};
