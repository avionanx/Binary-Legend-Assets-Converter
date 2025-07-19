#pragma once
#include <cmath>

template <typename T>
class Vector3 {
public:
	T x, y, z;

	const Vector3<T> add(const Vector3<T>& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}

	const Vector3<T> sub(const Vector3<T>& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	const Vector3<T> cross(const Vector3<T>& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}
	const Vector3<float> normalize() const {
		const float length = std::sqrt(x * x + y * y + z * z);
		if (length == 0.0f) {
			return { 0.0f, 0.0f, 0.0f };
		}
		return { x / length, y / length, z / length };
	}
};