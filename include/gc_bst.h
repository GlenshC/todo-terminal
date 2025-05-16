#ifndef GC_BINARY_SEARCH_TREE_H
#define GC_BINARY_SEARCH_TREE_H

typedef struct GCBNode{
    void *value;
    struct GCBNode *left;
    struct GCBNode *right;
} GCBNode, GCBRoot;

typedef int (*bstCmpFun)(const void *a, const void *b);
typedef void (*bstDispFun)(const void *value);

GCBRoot *gc_bst_init();
void gc_bst_free(GCBRoot **root);

void gc_bst_push(GCBRoot *root, void *value, bstCmpFun compare);
void gc_bst_display(GCBRoot *root, bstDispFun display);

#endif // GC_BINARY_SEARCH_TREE_H
