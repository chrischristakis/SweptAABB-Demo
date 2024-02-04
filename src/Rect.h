#pragma once

#include <glm/glm.hpp>

struct Rect {
	float x, y;
	float w, h;
	glm::vec3 color;

	// Constructors
	Rect(float x, float y, float w, float h, glm::vec3 color = {1, 1, 1}) : x(x), y(y), w(w), h(h), color(color) { }
	Rect() : Rect(0, 0, 0, 0) {}
};