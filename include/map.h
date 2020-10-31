#ifndef _MAP_H
#define _MAP_H

typedef struct _node{
    struct _node* next;
    char* key;
    void* value;
} node;

typedef struct _Map{
    int size;
    node** nodes;
} Map;

char *strMemCpy (char *s);
unsigned int makeHash(char* str, int length);

Map* NewMap(int size);
node* lookup(Map* map, char* key);
int GetData(Map* map, char* key, void** result, void(*dataCpy)(void **, void *));
int SetData(Map* map, char* key, void* value, void(*dataCpy)(void **, void *), void(*dataFree)(void *));
void FreeMap(Map* map, void(*dataFree)(void *));
void PrintMap(Map* map, void(*dataPrint)(void *));

void cpyString(void** des, void* src);
void freeString(void* src);
void printString(void* src);
void cpyMap(void** des, void* src);
void freeMap(void* src);

#endif