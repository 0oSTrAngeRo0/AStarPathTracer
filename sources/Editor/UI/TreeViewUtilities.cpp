#include "Editor/UI/TreeViewUtilities.h"
#include <imgui.h>

void TreeView::RecursivelyAddDirectoryNodes(TreeView::Node& parent, std::filesystem::directory_iterator directory_iterator) {
	for (const std::filesystem::directory_entry& entry : directory_iterator) {
		TreeView::Node& child = parent.children.emplace_back();
		child.id = entry.path().string();
		child.name = entry.path().filename().string();
		if (child.is_leaf = !entry.is_directory(); !child.is_leaf)
			RecursivelyAddDirectoryNodes(child, std::filesystem::directory_iterator(entry));
	}

	auto moveDirectoriesToFront = [](const TreeView::Node& a, const TreeView::Node& b) { return (a.is_leaf < b.is_leaf); };
	std::sort(parent.children.begin(), parent.children.end(), moveDirectoriesToFront);
}

void TreeView::RecursivelyDisplayDirectoryNode(const TreeView::Node& parent, TreeView::Result& result, const TreeView::NodeId& selected) {
	ImGui::PushID(&parent);
	if (parent.is_leaf) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth;

		// Todo: Can be optimized, string comparasion is too expensive!
		if (selected == parent.id) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::TreeNodeEx(parent.name.c_str(), flags);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			OnSelected(ImGuiMouseButton_Right, parent, result);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			OnSelected(ImGuiMouseButton_Left, parent, result);
		}
	}
	else {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
		if (selected == parent.id) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool is_open = ImGui::TreeNodeEx(parent.name.c_str(), flags);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			OnSelected(ImGuiMouseButton_Right, parent, result);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			OnSelected(ImGuiMouseButton_Left, parent, result);
		}
		if (is_open) {
			for (const Node& child : parent.children)
				RecursivelyDisplayDirectoryNode(child, result, selected);
			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}

void TreeView::OnSelected(uint32_t mouse_button, const TreeView::Node& node, TreeView::Result& result) {
	result.clicked = std::cref(node);
	result.mouse_button = ImGuiMouseButton_Right;
}

TreeView::Result TreeView::DrawUi(const TreeView::Node& root, const NodeId& selected) {
	TreeView::Result result;
	RecursivelyDisplayDirectoryNode(root, result, selected);
	return result;
}

TreeView::Result TreeView::DrawUiNoRoot(const Node& root, const NodeId& selected) {
	TreeView::Result result;
	for (const Node& node : root.children) {
		RecursivelyDisplayDirectoryNode(node, result, selected);
	}
	return result;
}

TreeView::Node TreeView::CreateDirectryNodeTreeFromPath(const std::filesystem::path& path) {
	TreeView::Node root;
	root.id = path.string();
	root.name = path.filename().string();
	if (root.is_leaf = !std::filesystem::is_directory(path); !root.is_leaf)
		RecursivelyAddDirectoryNodes(root, std::filesystem::directory_iterator(path));

	return root;
}
