#include "base.h"

template<typename F,typename...Args>
std::function<void()> mybind(F&&f,Args&&...args)
{
    std::function<void()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
    return func;
}
