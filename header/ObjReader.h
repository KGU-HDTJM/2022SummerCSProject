#pragma once

#if defined(OBJ_READER_EXPORT)
#define OBJ_READ_API __declspec(dllexport)
#else
#define OBJ_READ_API __declspec(dllimport)
#endif

extern OBJ_READ_API float* __cdecl GetVertexFromObjFileMalloc(const char* objFile);
