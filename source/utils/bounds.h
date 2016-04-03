#pragma once

#include <cmath>

enum BoundConstructLRTB {
	LRTB
};

struct Bounds {
	int left;
	int right;
	int top;
	int bottom;

	Bounds() : left(0), right(0), top(0), bottom(0) {};

	Bounds(int x, int y, int width, int height) : left(x), right(x+width), top(y), bottom(y+height)
		{};

	Bounds(BoundConstructLRTB, int l, int r, int t, int b) : left(l), right(r), top(t), bottom(b) {};

	Bounds(int width, int height) : left(0), right(width), top(0), bottom(height) {};

	bool contains(int x, int y) const {
		return (x >= left) && (y >= top) && (x <= right) && (y <= bottom);
	}

	bool contains(Bounds &other) {
		return (left < other.right) && (right > other.left) &&
				(top < other.bottom) && (bottom > other.top);
	}

	int width() const {
		return right-left;
	}

	int height() const {
		return bottom-top;
	}
	
	int centerX() const {
		return (left+right)/2;
	}
	
	int centerY() const {
		return (top+bottom)/2;
	}
	
	float radius() const {
		float halfWidth = ((float)width())/2.0f;
		float halfHeight = ((float)height())/2.0f;
		return sqrt(halfWidth*halfWidth + halfHeight*halfHeight);
	}

	void zero() {
		left = 0;
		right = 0;
		top = 0;
		bottom = 0;
	}

	void extend(const Bounds &other) {
		if (other.left < left) left = other.left;
		if (other.right > right) right = other.right;
		if (other.top < top) top = other.top;
		if (other.bottom > bottom) bottom = other.bottom;
	}

	void move(int x, int y) {
		left += x;
		right += x;
		top += y;
		bottom += y;
	}

	void clip(const Bounds &other) {
		if (left < other.left) left = other.left;
		if (right > other.right) right = other.right;
		if (top < other.top) top = other.top;
		if (bottom > other.bottom) bottom = other.bottom;
	}
};
