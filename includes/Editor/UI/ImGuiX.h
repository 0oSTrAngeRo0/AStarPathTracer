#pragma once

#include <imgui.h> 

namespace ImGui {
	bool InputFloatMatrix(
		const char* label, 
		float* data, 
		int rows, int columns, 
		const char* format = "%.3f", 
		ImGuiInputTextFlags flags = ImGuiInputFlags_None
	) {
		bool state = false;
		ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_DefaultOpen;
		if (!ImGui::TreeNodeEx(label, tree_node_flags)) return state;
		if (!ImGui::BeginTable(label, columns)) return state;
		for (int row = 0; row < rows; row++) {
			ImGui::TableNextRow();
			for (int column = 0; column < columns; column++) {
				int data_index = row * columns + column;
				ImGui::TableSetColumnIndex(column);
				ImGui::PushID(data_index);
				state |= ImGui::InputFloat("##cell", &data[data_index], 0.0f, 0.0f, format, flags);
				ImGui::PopID();
			}
		}
		ImGui::EndTable();
		return state;
	}
}
