#pragma once

#ifdef SVON_EXPORTS
#define SVON_API __declspec(dllexport)
#else
#define SVON_API __declspec(dllimport)
#endif