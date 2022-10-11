typedef struct entry {
    void * key;             // 键
    void * value;           // 值
    struct entry * next;    // 冲突链表
}*Entry;

#define newEntry() NEW(struct entry)
#define newEntryList(length) (Entry)malloc(length * sizeof(struct entry))

typedef struct hashMap *HashMap;
#define newHashMap() NEW(struct hashMap)

// 哈希函数类型
typedef int(*HashCode)(HashMap, void * key);

// 判等函数类型
typedef Boolean(*Equal)(void * key1, void * key2);

// 添加键函数类型
typedef void(*Put)(HashMap hashMap, void * key, void * value);

// 获取键对应值的函数类型
typedef void * (*Get)(HashMap hashMap, void * key);

// 删除键的函数类型
typedef Boolean(*Remove)(HashMap hashMap, void * key);

// 清空Map的函数类型
typedef void(*Clear)(HashMap hashMap);

// 判断键值是否存在的函数类型
typedef Boolean(*Exists)(HashMap hashMap, void * key);

typedef struct hashMap {
    int size;           // 当前大小
    int listSize;       // 有效空间大小
    HashCode hashCode;  // 哈希函数
    Equal equal;        // 判等函数
    Entry list;         // 存储区域
    Put put;            // 添加键的函数
    Get get;            // 获取键对应值的函数
    Remove remove;      // 删除键
    Clear clear;        // 清空Map
    Exists exists;      // 判断键是否存在
    Boolean autoAssign;	// 设定是否根据当前数据量动态调整内存大小，默认开启
}*HashMap;

// 默认哈希函数
static int defaultHashCode(HashMap hashMap, void * key);

// 默认判断键值是否相等
static Boolean defaultEqual(void * key1, void * key2);

// 默认添加键值对
static void defaultPut(HashMap hashMap, void * key, void * value);

// 默认获取键对应值
static void * defaultGet(HashMap hashMap, void * key);

// 默认删除键
static Boolean defaultRemove(HashMap hashMap, void * key);

// 默认判断键是否存在
static Boolean defaultExists(HashMap hashMap, void * key);

// 默认清空Map
static void defaultClear(HashMap hashMap);

// 创建一个哈希结构
HashMap createHashMap(HashCode hashCode, Equal equal);

// 重新构建
static void resetHashMap(HashMap hashMap, int listSize);