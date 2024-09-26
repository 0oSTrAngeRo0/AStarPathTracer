#pragma once

#include <entt/entt.hpp>
#include "Editor/UI/TreeViewUtilities.h"

class HierachiesPanel {
public:
	void DrawUi(const entt::registry& registry);
private:
	TreeView::State current_state;
};