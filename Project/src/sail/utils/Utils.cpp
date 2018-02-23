#include "Utils.h"

std::random_device Utils::rd;
std::mt19937 Utils::gen(Utils::rd());
std::uniform_real_distribution<float> Utils::dis(0.f, 1.f);

std::wstring Utils::vec3ToWStr(const DirectX::SimpleMath::Vector3& vec) {
	std::wstringstream ss;
	ss << "[X: " << vec.x << ", Y: " << vec.y << ", Z: " << vec.z << "]";
	return ss.str();
}

std::string Utils::vec3ToStr(const DirectX::SimpleMath::Vector3& vec) {
	std::stringstream ss;
	ss << "[X: " << vec.x << ", Y: " << vec.y << ", Z: " << vec.z << "]";
	return ss.str();
}

float Utils::rnd() {
	return dis(gen);
}

float Utils::clamp(float val, float min, float max) {

	if (val > max) return max;
	if (val < min) return min;
	return val;

}

float Utils::smootherstep(float edge0, float edge1, float x) {
	// Scale, and clamp x to 0..1 range
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	// Evaluate polynomial
	return x * x * x * (x * (x * 6 - 15) + 10);
}


DirectX::SimpleMath::Vector4 Utils::getRandomColor() {
	return DirectX::SimpleMath::Vector4(Utils::rnd(), Utils::rnd(), Utils::rnd(), 1);
}
