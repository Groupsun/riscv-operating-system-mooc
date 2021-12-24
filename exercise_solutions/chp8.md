# 练习

## 练习8-1

要求:参考 code/os/02-memanagement，在 page 分配的基础上实现更细颗粒度的，精确到字节为单位的内存管理。要求实现如下接口，具体描述参考`man malloc`：

```c
void *malloc(size_t size);
void free(void *ptr);
```

基本思路：使用第一个页来进行对malloc分配的内存进行管理。管理的主要方式是采用一个双向链表的形式来维护使用malloc进行分配的内存块的信息。链表中每个元素保存有使用malloc分配的内存块的起始地址以及大小。同时再维护一个空闲块的列表。