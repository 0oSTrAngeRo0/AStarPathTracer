#pragma once

#include <glm/glm.hpp>
#include <array>
#include <Utilities/EnumX.h>

class InputState {
public:
	enum class ActionState {
		ePress,
		eRepeat,
		eRelease,
	};

	enum class Keyboard {
		eA, eB, eC, eD, eE, eF, eG, eH, eI, eJ, eK, eL, eM, eN, eO, eP, eQ, eR, eS, eT, eU, eV, eW, eX, eY, eZ,
		eMaxValue
	};

	enum class MouseButton {
		eLeft,
		eMiddle,
		eRight,
		eMaxValue,
	};

	InputState() : mouse_position(0, 0), mouse_position_delta(0, 0), mouse_scroll(0, 0) {
		keyboard.fill(ActionState::eRelease);
		mouse_button.fill(ActionState::eRelease);
	}

	inline void SetKey(Keyboard key, ActionState state) { keyboard[GetEnumUnderlying<Keyboard>(key)] = state; }
	inline void SetMouseButton(MouseButton button, ActionState state) { keyboard[GetEnumUnderlying<MouseButton>(button)] = state; }
	inline void SetMousePosition(float x, float y) {
		glm::vec2 last = mouse_position;
		mouse_position.x = x;
		mouse_position.y = y;
		mouse_position_delta = mouse_position - last;
	}
	inline void SetMouseScroll(float x, float y) { mouse_scroll.x = x; mouse_scroll.y = y; }

	inline ActionState GetKey(Keyboard key) const { return keyboard[GetEnumUnderlying<Keyboard>(key)]; }
	inline ActionState GetMouseButton(MouseButton button) const { return keyboard[GetEnumUnderlying<MouseButton>(button)]; }
	inline glm::vec2 GetMousePosition() const { return mouse_position; }
	inline glm::vec2 GetMousePositionDelta() const { return mouse_position_delta; }
	inline glm::vec2 GetMouseScroll() const { return mouse_scroll; }

	inline void ClearFrameData() {
		mouse_scroll.x = 0;
		mouse_scroll.y = 0;
		mouse_position_delta.x = 0;
		mouse_position_delta.y = 0;
	}
private:
	std::array<ActionState, GetEnumUnderlying<Keyboard, Keyboard::eMaxValue>()> keyboard;
	std::array<ActionState, GetEnumUnderlying<MouseButton, MouseButton::eMaxValue>()> mouse_button;
	glm::vec2 mouse_position_delta;
	glm::vec2 mouse_position;
	glm::vec2 mouse_scroll;
};