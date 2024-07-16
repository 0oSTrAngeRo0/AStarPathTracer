#pragma once

#include "Engine/Resources/Resources.h"

class EditorInspectorBase {
public:
	virtual void DrawInspector() = 0;
};

class EmptyInspector : public EditorInspectorBase {
public:
	void DrawInspector() override {}
};