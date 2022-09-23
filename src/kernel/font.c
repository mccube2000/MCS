#include "types.h"
typedef struct {
    int8_t tag[4];
    int32_t checkSum;
    int32_t offset;
    int32_t length;
} TableEntry;

typedef struct {
    int32_t sfntversion; // 0x10000   for   version   1.0
    uint16_t numTables;
    uint16_t searchRange;
    uint16_t entrySelector;
    uint16_t rangeShift;
    TableEntry entries[1]; // variable   number   of   TableEntry
} TableDirectory;
