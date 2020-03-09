#pragma once

#include <memory>
#include <cstdio>
#include <string>

namespace SVON
{
	template<typename ... Args>
	static std::string formatString(const std::string& format, Args ... args)
	{
		auto size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::move(std::string(buf.get(), buf.get() + size - 1)); // We don't want the '\0' inside
	}

	template<typename ... Args>
	static std::wstring formatString(const std::wstring& format, Args ... args)
	{
		auto size = std::swprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
		std::swprintf(buf.get(), size, format.c_str(), args ...);
		return std::move(std::wstring(buf.get(), buf.get() + size - 1)); // We don't want the '\0' inside
	}


	template<typename ... Args>
	static void formatStringEx(std::string& dst, const std::string& format, Args ... args)
	{
		auto size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		dst = { buf.get(),  buf.get() + size - 1 };
	}

	template<typename ... Args>
	static void formatStringEx(std::wstring& dst, const std::wstring& format, Args ... args)
	{
		auto size = std::swprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
		std::swprintf(buf.get(), size, format.c_str(), args ...);
		dst = { buf.get(),  buf.get() + size - 1 };
	}
}