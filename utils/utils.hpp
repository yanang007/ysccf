#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <iterator>
#include <stack>

#define UNUSED(expr) do { (void)(expr); } while (0)

#define HAS_MEM_FUNC(FunctionName, HelperClassName) \
    template<typename U> \
    struct HelperClassName { \
    template <typename T, decltype(&T::FunctionName) f = &T::FunctionName> \
        static constexpr bool check(T*) { return true; } \
        static constexpr bool check(...) { return false; } \
        static constexpr bool value = check(static_cast<std::remove_reference_t<U>*>(nullptr)); \
    };

template < typename _Tp >
constexpr
inline bool alwaysFalse(_Tp){ return false; }
template < typename _Tp >
constexpr
inline bool alwaysTrue(_Tp t){ return !alwaysFalse<_Tp>(t); }

template <typename _Tp>
inline _Tp instantiate()
{ return *static_cast<_Tp*>(nullptr); }

template <typename _container,typename _Tp>
inline auto ufind_impl(const _container& cont,const _Tp& val, int)
-> decltype(cont.find(val))
{ return cont.find(val); }

template <typename _container,typename _Tp>
inline auto ufind_impl(const _container& cont,const _Tp& val, long)
{ return std::find(begin(cont),end(cont),val); }

template <typename _container,typename _Tp>
inline auto ufind(const _container& cont,const _Tp& val)
{ return ufind_impl(cont,val,0); }

template <typename _container, typename _Tp, typename _Func>
inline void ufindAllExec(const _container& cont, const _Tp& val, _Func _func)
{
    auto iter = std::find(begin(cont),end(cont),val);
    while ( iter != end(cont) ) {
        _func(iter);
        iter = std::find(++iter,end(cont),val);
    }
}

template <typename _container, class _Tp>
inline auto contains(const _container& cont, const _Tp& val)
-> decltype(end(cont), true)
{
    return end(cont) != ufind(cont,val);
}

template <typename _container>
inline auto& front(const _container& cont)
{ return *cont.begin(); }

template <typename _container>
inline auto& back(const _container& cont)
{ return *(--cont.end()); }

HAS_MEM_FUNC(pop_back,isDequeLike);

template <typename _container,bool _isDequeLike = isDequeLike<_container>::value>
struct toppopHelper{
    static auto doit(_container& cont);
};

template <typename _container>
struct toppopHelper<_container,true>{
    static auto doit(_container& cont){
        auto temp = cont.back();
        cont.pop_back();
        return temp;
    }
};

template <typename _container>
struct toppopHelper<_container,false>{
    static auto doit(_container& cont){
        auto temp = cont.top();
        cont.pop();
        return temp;
    }
};

template <typename _container>
inline auto toppop(_container& cont)
{ return toppopHelper<_container>::doit(cont); }

template <typename iterable>
iterable _union(const iterable& iia, const iterable& iib)
{
    iterable ret;
    std::set_union(iia.begin(),iia.end(),
                   iib.begin(),iib.end(),
                   std::inserter(ret,ret.begin()));
    return ret;
}

template <typename iterable>
iterable _intersect(const iterable& iia, const iterable& iib)
{
    iterable ret;
    std::set_intersection(iia.begin(),iia.end(),
                          iib.begin(),iib.end(),
                          std::inserter(ret,ret.begin()));
    return ret;
}

template <typename iterable>
iterable _difference(const iterable& iia, const iterable& iib)
{
    iterable ret;
    std::set_difference(iia.begin(),iia.end(),
                        iib.begin(),iib.end(),
                        std::inserter(ret,ret.begin()));
    return ret;
}


template <typename iterable, typename OutputIterator>
iterable _rangeOps(
        const iterable& iia,
        const iterable& iib,
        OutputIterator *_func(const iterable&,const iterable&))
{
    iterable ret;
    _func(iia.begin(),iia.end(),
          iib.begin(),iib.end(),
          std::inserter(ret,ret.begin()));
    return ret;
}

/*
#define HAS_MEM_FUNC(FunctionName, HelperClassName) \
    template<typename T> \
    struct HelperClassName { \
        typedef char one; \
        typedef long two; \
        template <typename C> static one test( decltype(&C::FunctionName)); \
        template <typename C> static two test( ...); \
    public: \
        enum { value = sizeof(test<T>(0)) == sizeof(char) }; \
    };
*/

// in progress
template <typename _Tp, size_t id = 0>
struct typeWrapper
{
    typedef typeWrapper<_Tp,id> thisType;
    _Tp tmp;
    typeWrapper() = default;
    explicit typeWrapper(const _Tp& rhv):tmp(rhv){}
    operator _Tp&() { return tmp; }
    operator const _Tp&() const { return tmp; }
    thisType& operator=(const _Tp& rhv) const { return (tmp=rhv),*this; }
};


#include <iostream>
HAS_MEM_FUNC(size,isIterableHelper);

template<typename cont,bool isIterable = isIterableHelper<cont>::value >
struct iterableHelper
{
    static void print(const cont& iterable);
};

template<typename cont>
struct iterableHelper<cont,false>
{
    static void print(const cont& iterable){
        std::cout<<iterable<<" ";
    }
};

template<typename cont>
struct iterableHelper<cont,true>
{
    static void print(const cont& iterable){
        std::cout<<"(";
        for( const auto& i : iterable){
            iterableHelper<decltype(i)>::print(i);
        }
        std::cout<<"),";
    }
};

#include <string>
template<typename cont>
void printContainer(const cont& c,std::wstring info = L"")
{
    /*
    for( auto& i : c){
        std::cout<<i<<" ";
    }
    */
    if ( !info.empty() )
        std::wcout<<info<<std::endl;
    iterableHelper<cont>::print(c);
    std::wcout<<std::endl;
}

template <typename iterable2, typename iterable1>
iterable2 container_cast(const iterable1& ia)
{
    return iterable2(ia.begin(),ia.end());
}

template<class _Elem,
    class _Traits = std::char_traits<_Elem>,
    class _Alloc = std::allocator<_Elem>>
std::basic_string<_Elem,_Traits,_Alloc>&
    replaced(std::basic_string<_Elem,_Traits,_Alloc>& str,
             const std::basic_string<_Elem,_Traits,_Alloc>& to_replaced,
             const std::basic_string<_Elem,_Traits,_Alloc>& newchars)
{

    for(size_t pos = 0; pos != str.npos; pos += newchars.length())
     {
        pos = str.find(to_replaced,pos);
        if(pos != str.npos)
            str.replace(pos,to_replaced.length(),newchars);
        else
            break;
     }
     return str;
 }

#ifdef _WIN32
#include <Windows.h>
inline BOOL IsObjectOnStack(LPVOID pObject)
{
    INT nStackValue(0);

    MEMORY_BASIC_INFORMATION mi = { 0 };
    DWORD dwRet = VirtualQuery(&nStackValue, &mi, sizeof(mi));

    if (dwRet > 0)
    {
        return pObject >= mi.BaseAddress
            && (DWORD)pObject < (DWORD)mi.BaseAddress + mi.RegionSize;
    }

    return FALSE;
}
#endif

#endif //UTILS_HPP
