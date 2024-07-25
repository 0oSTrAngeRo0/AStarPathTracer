#include "Editor/UI/FileBrowser.h"
#include "imgui.h"

void FileBrowser::RecursivelyAddDirectoryNodes(DirectoryNode& parent, std::filesystem::directory_iterator directoryIterator) {
	for (const std::filesystem::directory_entry& entry : directoryIterator) {
		DirectoryNode& child = parent.children.emplace_back();
		child.full_path = entry.path().string();
		child.file_name = entry.path().filename().string();
		if (child.is_directory = entry.is_directory(); child.is_directory)
			RecursivelyAddDirectoryNodes(child, std::filesystem::directory_iterator(entry));
	}

	auto moveDirectoriesToFront = [](const DirectoryNode& a, const DirectoryNode& b) { return (a.is_directory > b.is_directory); };
	std::sort(parent.children.begin(), parent.children.end(), moveDirectoriesToFront);
}

FileBrowser::DirectoryNode FileBrowser::CreateDirectryNodeTreeFromPath(const std::filesystem::path& path) {
	DirectoryNode root;
	root.full_path = path.string();
	root.file_name = path.filename().string();
	if (root.is_directory = std::filesystem::is_directory(path); root.is_directory)
		RecursivelyAddDirectoryNodes(root, std::filesystem::directory_iterator(path));

	return root;
}

void FileBrowser::RecursivelyDisplayDirectoryNode(const DirectoryNode& parent) {
	ImGui::PushID(&parent);
	if (parent.is_directory) {
		if (ImGui::TreeNodeEx(parent.file_name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth)) {
			for (const DirectoryNode& child : parent.children)
				RecursivelyDisplayDirectoryNode(child);
			ImGui::TreePop();
		}
	}
	else {
		auto flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanFullWidth;

		// Todo: Can be optimized, string comparasion is too expensive!
		if (current_selection.full_path == parent.full_path) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		ImGui::TreeNodeEx(parent.file_name.c_str(), flags);
		if (ImGui::IsItemClicked()) {
			if (current_selection.full_path != parent.full_path) {
				current_selection = parent;
				is_selection_changed = true;
			}
		}
	}
	ImGui::PopID();
}

FileBrowser::FileBrowser(const std::string path) {
	root = CreateDirectryNodeTreeFromPath(path);
}

void FileBrowser::OnDrawUi() {
	is_selection_changed = false;
	RecursivelyDisplayDirectoryNode(root);
}
