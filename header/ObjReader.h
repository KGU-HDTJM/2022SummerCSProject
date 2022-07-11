#pragma once

#if defined OBJ_READER_EXPORT
#define OBJ_READ_API __declspec(dllexport)
#else
#define OBJ_READ_API __declspec(dllimport)
#endif

extern OBJ_READ_API size_t __cdecl GetVertexLength(FILE* objFile);
extern OBJ_READ_API float* __cdecl GetVertexFromObjFileMalloc(const char* objFile, size_t* length);
