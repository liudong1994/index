#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstdint>
#include <string>
#include <unistd.h>
#include <sys/time.h>
#include <functional>


namespace dindex
{

// #define _DEBUG

const int32_t INDEX_ERROR              = -1;   // 操作失败
const int32_t INDEX_OK                 = 0;    // 操作成功


// 避免编译告警
#define UNUSED(x) (void)(x)


// delete
template<typename POINTER>
inline void SAFE_DELETE(POINTER &pointer) 
{
    if (pointer) {
        delete(pointer);
        pointer = NULL;
    }
}

template<typename ARRPOINTER>
inline void SAFE_DELETE_ARRAY(ARRPOINTER &arrPointer) 
{
    if (arrPointer) {
        delete []arrPointer;
        arrPointer = NULL;
    }
}

template<typename POINTER>
inline void SAFE_FREE(POINTER &pointer) 
{
    if (pointer) {
        free(pointer);
        pointer = NULL;
    }
}

template<typename FD>
inline void SAFE_CLOSE(FD &fd) 
{
    if (fd > 0) {
        close(fd);
        fd = -1;
    }
}


// time
inline uint64_t GET_CURRENTTIME_MS() 
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (uint64_t)(tval.tv_sec)*1000 + tval.tv_usec/1000;
}

inline uint64_t GET_CURRENTTIME_US() 
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (uint64_t)(tval.tv_sec)*1000000 + tval.tv_usec;
}


/* 
    原理：RAII, 局部变量存储在栈中, 在析构局部变量时, 调用析构函数执行一些资源释放的操作
    限制：
        局部变量在栈中是先进后出, 所以后面的defer先于前面的defer被调用, 同时也需要保证defer函数中使用的临时变量需要在defer前定义
*/
class CoDeferHelper
{
public:
	CoDeferHelper(std::function<void()> &&func) : m_func(std::move(func)) {}
    CoDeferHelper() {}
	~CoDeferHelper() { if (m_func) m_func(); }

    void set_func(std::function<void()> &&func) { m_func=std::move(func); }

private:
	std::function<void()> m_func;
};

#define CO_DEFER_CONNECTION(text1,text2) text1##text2
#define CO_DEFER_CONNECT(text1,text2) CO_DEFER_CONNECTION(text1,text2)
#define co_defer(func)  CoDeferHelper CO_DEFER_CONNECT(L,__LINE__) ([&](){func;});      // 引用捕获

/*
    需求：提前定义refer函数, 但是在 {return 函数; } 语句中, 希望先执行资源释放函数, 在执行return后的函数
    原理：最开始处定义CO_DEFER_RETURN, 最后return处替换func, 利用栈变量最后释放CO_DEFER_RETURN, 达到目的
    限制：
        因为首先定义的CO_DEFER_RETURN, 所以在co_defer_return中使用的变量必须在co_use_defer_return()之前定义, 否则可能产生未定义行为
        为了避免出现上面的情况 lambda采用值捕获 拷贝函数参数（尽量使用指针）
*/
#define co_use_defer_return()  CoDeferHelper CO_DEFER_RETURN;
#define co_defer_return(func)  CO_DEFER_RETURN.set_func ([=](){func;});     // 值捕获

}

#endif //_COMMON_H_

