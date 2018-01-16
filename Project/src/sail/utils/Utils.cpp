#include "Utils.h"

std::random_device Utils::rd;
std::mt19937 Utils::gen(Utils::rd());
std::uniform_real_distribution<float> Utils::dis(0.f, 1.f);

std::wstring Utils::vec3ToWStr(const DirectX::SimpleMath::Vector3& vec) {
	std::wstringstream ss;
	ss << "[X: " << vec.x << ", Y: " << vec.y << ", Z: " << vec.z << "]";
	return ss.str();
}

float Utils::rnd() {
	return dis(gen);
}
