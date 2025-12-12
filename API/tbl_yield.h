#ifndef _TBL_YIELD_H_
#define _TBL_YIELD_H_

#define CORO_CTX(name) \
    static void * __coro_##name = NULL

#define CORO_BEGIN(name) \
    CORO_CTX(name); \
    if (__coro_##name == NULL) { \
        __coro_##name = &&__coro_start_##name; \
    } \
    goto *__coro_##name; \
    __coro_start_##name:


#define CORO_END(name) \
    __coro_##name = NULL

#define CORO_CONCAT(a, b, c) a##b##c

#define CORO_YIELD_IMPL(name, codelin) \
    do { \
        __coro_##name = &&CORO_CONCAT(__coro_resume_, name, codelin); \
        return; \
        CORO_CONCAT(__coro_resume_, name, codelin):; \
    } while (0)

#define CORO_YIELD(name) CORO_YIELD_IMPL(name, __LINE__)

#define CORO_WAIT_UNTIL(name)

#endif
