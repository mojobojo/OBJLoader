/*
    OBJ Loader - mojobojo 2016
        - Built to load obj files expored by blender
        - Built in stb style
        - Lets you use your own block of allocated memory to push the data onto
        - Outputs a struct that exposes the data as you would see
          inside an obj file.
        - Currently uses a incomplete obj file spec
        - Uses 4 stdlib functions
            - malloc (only if you want to use the function that uses it)
            - free (onlt if you use the function with malloc)
            - atof
            - strtoul

        - TODO:
            - Complete obj file spec
                - mtl loading
                - load 4-float vertexes
                - load single index value faces (right now only loads the format 0/0/0)
*/

#ifndef __OBJ_LOADER_H_
#define __OBJ_LOADER_H_
#include <stdlib.h>
#include <stdint.h>

typedef int8_t   objl_i8;
typedef uint8_t  objl_u8;
typedef int16_t  objl_i16;
typedef uint16_t objl_u16;
typedef int32_t  objl_i32;
typedef uint32_t objl_u32;
typedef int64_t  objl_i64;
typedef uint64_t objl_u64;
typedef float    objl_f32;
typedef double   objl_f64;
typedef objl_i32 objl_b32;

#define objl_internal static
#define objl_EatLine() while (*b != '\n') { ++b; } ++b

typedef struct {
    size_t Used;
    size_t Size;
    void *Memory;
} objl_memory;

typedef struct {
    objl_f32 x;
    objl_f32 y;
    objl_f32 z;
} objl_v3;

typedef struct {
    objl_f32 x;
    objl_f32 y;
} objl_v2;

typedef struct {
    objl_i32 vertex;
    objl_i32 texture;
    objl_i32 normal;
} objl_f_element;

typedef struct {
    objl_f_element f0;
    objl_f_element f1;
    objl_f_element f2;
} objl_f;

typedef struct {
    char *o;
    objl_i32 v_count;
    objl_v3 *v;
    objl_i32 vt_count;
    objl_v2 *vt;
    objl_i32 vn_count;
    objl_v3 *vn;
    char *s;
    objl_i32 f_count;
    objl_f *f;

    objl_memory Memory;
} objl_obj_file;

#ifdef __cplusplus
extern "C"
{
#endif

// Use this if you wish to use your own allocated block of memory to put the obj data in.
// The obj loader uses an internal arena allocater.
objl_internal void objl_LoadObj(char *ObjData, void *ObjMemory, size_t ObjMemorySize, objl_obj_file *ObjFileOut);

// If you do not care to use your own allocated block of memory
// you can use this version that uses malloc
objl_internal void objl_LoadObjMalloc(char *ObjData, objl_obj_file *ObjFileOut);

// This should only be used if you used objl_LoadObjMalloc
objl_internal void objl_FreeObj(objl_obj_file *ObjFileOut);

#ifdef OBJL_IMPLEMENTATION

objl_internal void *
objl_PushMemory(objl_memory *Memory, size_t Amount) {
    void *Result = 0;
    if ((Memory->Used + Amount) < Memory->Size) {
        Result = (objl_u8 *)Memory->Memory + Memory->Used;
        Memory->Used += Amount;
    }
    return Result;
}

objl_internal void
objl_CopyMemory(void *Destination, void *Source, size_t Length) {
    objl_u8 *d = (objl_u8 *)Destination;
    objl_u8 *s = (objl_u8 *)Source;
    while (Length--) {
        *d++ = *s++;
    }
}

objl_internal void
objl_ZeroMemory(void *Destination, size_t Length) {
    objl_u8 *d = (objl_u8 *)Destination;
    while (Length--) {
        *d++ = 0;
    }
}

objl_internal size_t
objl_strlen(char *String) {
    size_t n = 0;
    while (*String++) {
        ++n;
    }
    return n;
}

objl_internal objl_f32
objl_StringToFloat(char *String) {
    // TODO(joey): My own string to float implementation.
    return (objl_f32)atof(String);
}

objl_internal objl_u32
objl_StringToUInt(char *String) {
    // TODO(joey): My own string to int implementation.
    objl_u32 Value = 0;
    if (objl_strlen(String)) {
        Value = (objl_u32)strtoul(String, 0, 0);
    }
    return Value;
}

objl_internal void
objl_ParseObj(char *ObjData, objl_obj_file *ObjFileOut, objl_b32 UseMalloc) {
    char *b = ObjData;

    // NOTE(joey): Count the vertexes, normals, and faces.
    while (*b) {
        if (*b == 'v') {
            if (*(b+1) == 'n') {
                ++ObjFileOut->vn_count;
            }
            else if (*(b+1) == 't') {
                ++ObjFileOut->vt_count;
            }
            else {
                ++ObjFileOut->v_count;
            }
        }
        else if (*b == 'f') {
            ++ObjFileOut->f_count;
        }

        while (*b != '\n' && *b != '\0') { 
            ++b; 
        }

        // NOTE(joey): In case we reach a null char before a new line
        if (*b) {
            ++b;
        }
    }

    // NOTE(joey): The OBJ file spec says there can be 4 elements in a row for verts. The OBJ files
    // blender puts out has not had them so I have not bothered to implement it.
    if (UseMalloc) {
        ObjFileOut->v = (objl_v3 *)malloc(ObjFileOut->v_count*sizeof(objl_v3));
        ObjFileOut->vt = (objl_v2 *)malloc(ObjFileOut->vt_count*sizeof(objl_v2));
        ObjFileOut->vn = (objl_v3 *)malloc(ObjFileOut->vn_count*sizeof(objl_v3));
        ObjFileOut->f = (objl_f *)malloc(ObjFileOut->f_count*sizeof(objl_f));
    }
    else {
        ObjFileOut->v = (objl_v3 *)objl_PushMemory(&ObjFileOut->Memory, ObjFileOut->v_count*sizeof(objl_v3));
        ObjFileOut->vt = (objl_v2 *)objl_PushMemory(&ObjFileOut->Memory, ObjFileOut->vt_count*sizeof(objl_v2));
        ObjFileOut->vn = (objl_v3 *)objl_PushMemory(&ObjFileOut->Memory, ObjFileOut->vn_count*sizeof(objl_v3));
        ObjFileOut->f = (objl_f *)objl_PushMemory(&ObjFileOut->Memory, ObjFileOut->f_count*sizeof(objl_f));
    }

    objl_v3 *v = ObjFileOut->v;
    objl_v2 *vt = ObjFileOut->vt;
    objl_v3 *vn = ObjFileOut->vn;

    objl_f_element *f = (objl_f_element *)ObjFileOut->f;

    b = ObjData;
    while (*b) {
        if (*b == '#') {
            objl_EatLine();
        }
        else if (*b == 'm') {
            //mtllib
            if (*(b+1) == 't' &&
                    *(b+2) == 'l' &&
                    *(b+3) == 'l' &&
                    *(b+4) == 'i' &&
                    *(b+5) == 'b') {
                // TODO(joey): mtl handling
                objl_EatLine();
            }
        }
        else if (*b == 'o') {
            ++b; ++b;
            char *s = b;
            while (*b != '\n') {
                ++b;
            }
            if (UseMalloc) {
                ObjFileOut->o = (char *)malloc(b-s+1);
            }
            else {
                ObjFileOut->o = (char *)objl_PushMemory(&ObjFileOut->Memory, b-s+1);
            }
            objl_CopyMemory(ObjFileOut->o, s, b-s);
            ObjFileOut->o[b-s] = 0;
            ++b;
        }
        else if (*b == 'v') {
            if (*(b+1) == 'n') {
                ++b; ++b; ++b;
                objl_f32 vec[3];
                objl_f32 *temvec = vec;
                while (*b != '\n') {
                    char TmpBuf[32];
                    objl_ZeroMemory(TmpBuf, 32);
                    char *t = TmpBuf;
                    while (*b != ' ') {
                        if (*b == '\n') {
                            break;
                        }
                        else {
                            *t++ = *b++;
                        }
                    }
                    if (*b == ' ') {
                        ++b;
                    }
                    objl_f32 Value = objl_StringToFloat(TmpBuf);
                    *temvec++ = Value;
                }
                vn->x = vec[0];
                vn->y = vec[1];
                vn->z = vec[2];
                ++vn;
                ++b;
            }
            else if (*(b+1) == 't') {
                ++b; ++b; ++b;
                objl_f32 vec[3];
                objl_f32 *temvec = vec;
                while (*b != '\n') {
                    char TmpBuf[32];
                    objl_ZeroMemory(TmpBuf, 32);
                    char *t = TmpBuf;
                    while (*b != ' ') {
                        if (*b == '\n') {
                            break;
                        }
                        else {
                            *t++ = *b++;
                        }
                    }
                    if (*b == ' ') {
                        ++b;
                    }
                    objl_f32 Value = objl_StringToFloat(TmpBuf);
                    *temvec++ = Value;
                }
                vt->x = vec[0];
                vt->y = vec[1];
                ++vt;
                ++b;
            }
            else {
                ++b; ++b;
                objl_f32 vec[3];
                objl_f32 *temvec = vec;
                while (*b != '\n') {
                    char TmpBuf[32];
                    objl_ZeroMemory(TmpBuf, 32);
                    char *t = TmpBuf;
                    while (*b != ' ') {
                        if (*b == '\n') {
                            break;
                        }
                        else {
                            *t++ = *b++;
                        }
                    }
                    if (*b == ' ') {
                        ++b;
                    }
                    objl_f32 Value = objl_StringToFloat(TmpBuf);
                    *temvec++ = Value;
                }
                v->x = vec[0];
                v->y = vec[1];
                v->z = vec[2];
                ++v;
                ++b;
            }
        }
        else if (*b == 'u') {
            // usemtl
            if (*(b+1) == 's' &&
                    *(b+2) == 'e' &&
                    *(b+3) == 'm' &&
                    *(b+4) == 't' &&
                    *(b+5) == 'l') {
                objl_EatLine();
            }
        }
        else if (*b == 'f') {
            ++b; ++b;
            while (*b != '\n') {
                char TmpBuf[32];
                objl_ZeroMemory(TmpBuf, 32);
                char *t = TmpBuf;
                while (*b != ' ') {
                    if (*b == '\n') {
                        break;
                    }
                    else {
                        *t++ = *b++;
                    }
                }
                if (*b == ' ') {
                    ++b;
                }

                objl_i32 sc = 0;
                char *n = TmpBuf;
                char *c = n;
                char *c1 = "0";
                char *c2 = "0";
                char *c3 = "0";
                while (*n) {
                    if (*n == '/') {
                        if (sc == 0) {
                            *n = 0;
                            c1 = c;
                            c = n+1;
                        }
                        else if (sc == 1) {
                            *n = 0;
                            c2 = c;
                            c = n+1;
                            c3 = c;
                        }
                        ++sc;
                    }
                    ++n;
                }

                f->vertex = objl_StringToUInt(c1); 
                f->texture = objl_StringToUInt(c2);
                f->normal = objl_StringToUInt(c3);
                *f++;
            }
            ++b;
        }
        else if (*b == 's') {
            ++b; ++b;
            char *s = b;
            while (*b != '\n') {
                ++b;
            }
            if (UseMalloc) {
                ObjFileOut->s = (char *)malloc(b-s+1);
            }
            else {
                ObjFileOut->s = (char *)objl_PushMemory(&ObjFileOut->Memory, b-s+1);
            }
            objl_CopyMemory(ObjFileOut->s, s, b-s);
            ObjFileOut->s[b-s] = 0;
            ++b;
        }
        else {
            ++b;
        }
    }
}

objl_internal void
objl_LoadObjMalloc(char *ObjData, objl_obj_file *ObjFileOut) {
    if (ObjData && ObjFileOut) {
        objl_ZeroMemory(ObjFileOut, sizeof(objl_obj_file));
        objl_ParseObj(ObjData, ObjFileOut, true);
    }
}

objl_internal void
objl_FreeObj(objl_obj_file *ObjFile) {
    free(ObjFile->o);
    free(ObjFile->v);
    free(ObjFile->vt);
    free(ObjFile->vn);
    free(ObjFile->s);
    free(ObjFile->f);
    objl_ZeroMemory(ObjFile, sizeof(objl_obj_file));
}

// Takes a null terminated buffer with the obj data.
objl_internal void
objl_LoadObj(char *ObjData, void *ObjMemory, size_t ObjMemorySize, objl_obj_file *ObjFileOut) {
    if (ObjData && ObjMemory && ObjMemorySize && ObjFileOut) {
        objl_ZeroMemory(ObjFileOut, sizeof(objl_obj_file));

        ObjFileOut->Memory.Used = 0;
        ObjFileOut->Memory.Size = ObjMemorySize;
        ObjFileOut->Memory.Memory = ObjMemory;

        objl_ParseObj(ObjData, ObjFileOut, false);
    }
}

#endif //OBJL_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif

