#pragma once

#include <Windows.h>
#include <exception>
#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include <d3d11.h>
#include <SimpleMath.h>

// Break on failed HRESULT return
#define ThrowIfFailed(result)	\
	if (FAILED(result))			\
		throw std::exception(); \

class Memory {

public:
	template <typename T>
	static void safeDelete(T& t) {
		if (t) {
			delete t;
			t = nullptr;
		}
	}

	template <typename T>
	static void safeDeleteArr(T& t) {
		if (t) {
			delete[] t;
			t = nullptr;
		}
	}

	template<typename T>
	static void safeRelease(T& t) {
		if (t) {
			t->Release();
			t = nullptr;
		}
	}

};

class Logger {

public:
	inline static void Warning(const std::string& msg) {
		
		HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

		// Save currently set color
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hstdout, &csbi);

		SetConsoleTextAttribute(hstdout, 0xE0);
		std::cout << "WARNING: " << msg << std::endl;

		// Revert color
		SetConsoleTextAttribute(hstdout, csbi.wAttributes);

#ifdef _SAIL_THROW_ON_WARNING
#ifdef _DEBUG
		throw std::exception();
#endif
#endif
	}

	inline static void Error(const std::string& msg) {

		HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

		// Save currently set color
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hstdout, &csbi);

		SetConsoleTextAttribute(hstdout, 0xC0);
		std::cout << "ERROR: " << msg << std::endl;

		// Revert color
		SetConsoleTextAttribute(hstdout, csbi.wAttributes);

#ifdef _SAIL_THROW_ON_ERROR
#ifdef _DEBUG
		throw std::exception();
#endif
#endif
	}

};

class Utils {
public:
	static std::wstring vec3ToWStr(const DirectX::SimpleMath::Vector3& vec);
	static float rnd();

	static std::random_device rd;
	static std::mt19937 gen;
	static std::uniform_real_distribution<float> dis;

};