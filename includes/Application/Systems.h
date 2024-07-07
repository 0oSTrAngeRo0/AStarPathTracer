#pragma once

#include <entt/entt.hpp>

class RenderContext;

class Systems {
private:
	entt::registry registry;
public:
	Systems(RenderContext& renderer);
	void Update(float time);
	inline entt::registry& GetRegistry() { return registry; }
	~Systems();
};