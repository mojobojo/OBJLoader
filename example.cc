#include <stdio.h>

#define OBJL_IMPLEMENTATION
#include "obj_loader.h"

int
main(int argc, char **argv) {
    FILE *f = fopen("test_cube.obj", "rb");
    objl_obj_file ObjFile;

    if (f) {
        fseek(f, 0, SEEK_END);
        long int FileSize = ftell(f);
        ftell(f);
        rewind(f);

        char *FileData = (char *)malloc(FileSize+1);
        fread(FileData, 1, FileSize, f);
        fclose(f);

        // The obj loader requires data to be null terminated
        FileData[FileSize] = 0;

#if 0
        size_t MemorySize = 1024 * 1024;
        void *Memory = malloc(MemorySize);

        objl_LoadObj(FileData, Memory, MemorySize, &ObjFile);
#else
        objl_LoadObjMalloc(FileData, &ObjFile);
#endif

        printf("Loaded OBJ File:\n");
        printf("    o:        %s\n", ObjFile.o);
        printf("    v_count:  %d\n", ObjFile.v_count);
        for (int i = 0; i < ObjFile.v_count; ++i) {
            printf("        x %f y %f z %f\n", ObjFile.v[i].x, ObjFile.v[i].y, ObjFile.v[i].z);
        }
        printf("\n");

        printf("    vt_count: %d\n", ObjFile.vt_count);
        for (int i = 0; i < ObjFile.vt_count; ++i) {
            printf("        x %f y %f\n", ObjFile.vt[i].x, ObjFile.vt[i].y);
        }
        printf("\n");

        printf("    vn_count: %d\n", ObjFile.vn_count);
        for (int i = 0; i < ObjFile.vn_count; ++i) {
            printf("        x %f y %f z %f\n", ObjFile.vn[i].x, ObjFile.vn[i].y, ObjFile.vn[i].z);
        }
        printf("\n");

        printf("    s:        %s\n", ObjFile.s);

        printf("    f_count:  %d\n", ObjFile.f_count);
        for (int i = 0; i < ObjFile.f_count; ++i) {
            printf("    f%d\n", i);
            printf("        0:\n");
            printf("            vertex:  %d\n", ObjFile.f[i].f0.vertex);
            printf("            texture: %d\n", ObjFile.f[i].f0.texture);
            printf("            normal:  %d\n", ObjFile.f[i].f0.normal);
            printf("        1:\n");
            printf("            vertex:  %d\n", ObjFile.f[i].f1.vertex);
            printf("            texture: %d\n", ObjFile.f[i].f1.texture);
            printf("            normal:  %d\n", ObjFile.f[i].f1.normal);
            printf("        2:\n");
            printf("            vertex:  %d\n", ObjFile.f[i].f2.vertex);
            printf("            texture: %d\n", ObjFile.f[i].f2.texture);
            printf("            normal:  %d\n", ObjFile.f[i].f2.normal);
        }
        printf("\n");

        objl_FreeObj(&ObjFile);
    }

    return 0;
}

