#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* strMemCpy (char* str) {
    char* res;

    res = (char*)malloc(strlen(str)+1);
    if (res != NULL)
        strcpy (res, str);
    return res;
}

unsigned int makeHash(char* str, int length) {
    unsigned int hashval = 0;

    for (; *str != '\0'; str++)
        hashval = *str + 31 * hashval;

    return hashval % length;
}

Map* NewMap(int size) {
    Map* map = (Map*)malloc(sizeof(Map));
    map->nodes = (node**)malloc(sizeof(node*) * size);
    map->size = size;

    for (int i = 0; i < size; i++)
        map->nodes[i] = NULL;

    return map;
}

node* lookup(Map* map, char* key) {
    if (map == NULL || key == NULL) return NULL;
    node* iter;

    for (iter = map->nodes[makeHash(key, map->size)]; iter != NULL; iter = iter->next) {
        if (strcmp(key, iter->key) == 0)
            return iter;
    }

    return NULL;
}

int GetData(Map* map, char* key, void** result, void(*dataCpy)(void **, void *)) {
    if (map == NULL || key == NULL) return -1;
    node* iter;
    
    iter = lookup(map, key);
    if (iter != NULL) {
        dataCpy(result, iter->value);
        return 0;
    }

    return -1;
}

int SetData(Map* map, char* key, void* value, void(*dataCpy)(void **, void *), void(*dataFree)(void *)) {
    if (map == NULL || key == NULL || value == NULL) return -1;
    node* iter;
    unsigned int hashval = makeHash(key, map->size);
    
    iter = lookup(map, key);
    if (iter == NULL) {
        iter = (node*)malloc(sizeof(node));
        if (iter == NULL || (iter->key = strMemCpy(key)) == NULL) 
            return -1;
        dataCpy(&(iter->value), value);
        iter->next = map->nodes[hashval];
        map->nodes[hashval] = iter;
    } else {
        dataFree(iter->value);
        dataCpy(&(iter->value), value);
    }

    return 0;
}

void FreeMap(Map* map, void(*dataFree)(void *)) {
    if (map == NULL) return;
    node *iter, *next;

    for (int i = 0; i < map->size; i++) {
        for (iter = map->nodes[i]; iter != NULL;) {
            free(iter->key);
            dataFree(iter->value);
            next = iter->next;
            free(iter);
            iter = next;
        }
    }
    free(map->nodes);
    free(map);
}

void PrintMap(Map* map, void(*dataPrint)(void *)) {
    if (map == NULL) return;
    node *iter;

    for (int i = 0; i < map->size; i++) {
        iter = map->nodes[i];
        for (iter = map->nodes[i]; iter != NULL; iter = iter->next) {
            printf("key:%-20s value:", iter->key);
            dataPrint(iter->value);
            printf("\n");
        }
    }
}

void cpyString(void** des, void* src) {
    *des = (void *)strMemCpy((char*)src);
}

void freeString(void* src) {
    free((char *)src);
}

void printString(void* src) {
    printf("%s", (char *)src);
}

void cpyMap(void** des, void* src) {
    *des = src;
}

// 어차피 포인터만 연결했기 때문에 아무것도 안할거임
void freeMap(void* src) {

}