#pragma once

#if defined OBJ_READER_EXPORT
#define OBJ_READ_API __declspec(dllexport)
#else
#define OBJ_READ_API __declspec(dllimport)
#endif

extern OBJ_READ_API const float* __cdecl GetVertexFromObjFileMalloc(const char* objFile, size_t* length);
extern OBJ_READ_API void __cdecl ReleaseObjVertex(float* vertex);