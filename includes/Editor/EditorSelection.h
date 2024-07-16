#pragma once

#include <string>
#include <memory>

class EditorInspectorBase;

class EditorSelection {
private:
	std::shared_ptr<EditorInspectorBase> selected_inspector;
public:
	EditorSelection();
	void SelectResource(const std::string& path);
	EditorInspectorBase& GetSelectedInspector() const { return *selected_inspector; }
};