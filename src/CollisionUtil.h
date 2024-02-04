#pragma once

#include <glm/glm.hpp>
#include <limits>
#include "Rect.h"

const float INF = std::numeric_limits<float>::infinity();

struct SweptCollisionInfo {
	bool collided;
	float collisionTime;
	glm::vec2 collisionNormal;
};

SweptCollisionInfo SweptAABB(const Rect& moving, const Rect& fixed, glm::vec2 velocity) {
	SweptCollisionInfo info;

	if (velocity.x == 0 && velocity.y == 0) {
		info.collided = false;
		return info;
	}

	// Calculate the distances for the entry and exit point on both axis
	// Note these distances are different depending on the direction of velocity
	float entryDistanceX, entryDistanceY;
	float exitDistanceX, exitDistanceY;

	if (velocity.x > 0) {
		entryDistanceX = fixed.x - (moving.x + moving.w);
		exitDistanceX = (fixed.x + fixed.w) - moving.x;
	}
	else if (velocity.x < 0) {
		entryDistanceX = (fixed.x + fixed.w) - moving.x;
		exitDistanceX = fixed.x - (moving.x + moving.w);
	}
	else {
		if (moving.x + moving.w < fixed.x || moving.x > fixed.x + fixed.w) {
			info.collided = false;
			return info;
		}
	}

	if (velocity.y > 0) {
		entryDistanceY = fixed.y - (moving.y + moving.h);
		exitDistanceY = (fixed.y + fixed.h) - moving.y;
	}
	else if (velocity.y < 0) {
		entryDistanceY = (fixed.y + fixed.h) - moving.y;
		exitDistanceY = fixed.y - (moving.y + moving.h);
	}
	else {
		if (moving.y + moving.h < fixed.y || moving.y > fixed.y + fixed.h) {
			info.collided = false;
			return info;
		}
	}

	// Calculate entry time on each axis.
	float entryTimeX = -INF,
		   exitTimeX = INF;
	float entryTimeY = -INF,
		   exitTimeY = INF;

	if (velocity.x != 0) {
		entryTimeX = entryDistanceX / velocity.x;
		exitTimeX = exitDistanceX / velocity.x;
	}

	if (velocity.y != 0) {
		entryTimeY = entryDistanceY / velocity.y;
		exitTimeY = exitDistanceY / velocity.y;
	}

	float entryTime = std::max(entryTimeX, entryTimeY);
	float exitTime = std::min(exitTimeX, exitTimeY);

	if (entryTime > exitTime || entryTime < 0 || entryTime > 1) {
		info.collided = false;
		return info;
	}

	info.collided = true;
	info.collisionTime = entryTime;
	return info;
}

bool SimplePointAABB(const Rect& r, glm::vec2 point) {
	return (
		point.x >= r.x &&
		point.x <= r.x + r.w &&
		point.y >= r.y &&
		point.y <= r.y + r.h
	);
}