#include "DATA_Handle.h"

int Find_Data(const U8 *source, char Target, int Length)
{
    for (size_t i = 0; i < Length;)
    {
        if (Target == source[i++])
        {
            return i;
        }
    }
    return 0;
}
