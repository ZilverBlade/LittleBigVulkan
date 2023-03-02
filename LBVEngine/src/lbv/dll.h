#pragma once

#ifdef _WIN32
#ifdef LBVENGINEAPI_DLLEXPORT
#define LBVAPI __declspec(dllexport)
#else
#define LBVAPI __declspec(dllimport)
#endif
#else
#define LBVAPI
#endif