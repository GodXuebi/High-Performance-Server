#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class noncopyable
{
protected:
    noncopyable(){}
    ~noncopyable(){}
private:
    noncopyable(const noncopyable&other);
    const noncopyable&operator=(const noncopyable&other);

};

#endif