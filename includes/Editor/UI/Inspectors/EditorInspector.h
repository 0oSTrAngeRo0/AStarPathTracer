#pragma once

class EditorInspectorBase {
public:
	virtual void DrawInspector() = 0;
	virtual ~EditorInspectorBase() = default;
};

class EmptyInspector : public EditorInspectorBase {
public:
	void DrawInspector() override {}
};