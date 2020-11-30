#ifndef INVOKEFUL_HPP
#define INVOKEFUL_HPP

#include <tuple>
#include <functional>
#include <vector>

/*
 *
#include "utils/invoker.hpp"

struct ss
{
    ss(){ std::cout<<"construct\n"; }
    ss(const ss&){ std::cout<<"copy construct\n"; }
    ss(ss&&){ std::cout<<"move construct\n"; }
    int operator()(int) {  return 1; }
};

int adde(int& i, int* j)
{
    return i += *j;
}

auto x = invoking(adde);
auto o = overload(std::move(x),ss());
int i = 0, j = 10;
std::tuple tup(std::ref(i),&j);
std::cout<<o(tup)<<o(i);

*/


template < typename _T >
auto decay_forward(_T&& arg)
{
    return std::forward<std::decay_t<_T>>(arg);
}

template <typename _Fn>
class invoking : public _Fn
{
public:
    invoking(_Fn&& f) : _Fn(f) {}

    template < typename... Args >
    auto invoke(const std::tuple<Args...>& tup)
    {
        return tupleInvokeImpl(tup,std::make_index_sequence<sizeof...(Args)>());
    }

    template < typename... Args >
    auto operator()(const std::tuple<Args...>& tup)
    {
        return invoke(tup);
    }

    /*template < typename Iter, typename IterEnd >
    auto invoke(Iter iter, IterEnd end)
    {
        return anyInvokeImpl(iter,end)();
    }*/

private:
    template < class TupType, size_t... I >
    auto tupleInvokeImpl(const TupType& tup,std::index_sequence<I...>)
    {
        /*return _Fn::operator() (
            std::forward<
                std::decay_t<
                    decltype(std::get<I...>(tup)) >>
            (std::get<I...>(tup)) );*/
        return _Fn::operator() (decay_forward(std::get<I>(tup))...);
        //return std::invoke(&_Fn::operator(), this, std::get<I>(tup)...);
    }

    /*template <typename Iter, typename IterEnd>
    auto anyInvokeImpl(Iter iter, IterEnd end)
    {
        if( iter == end ){
            return [&](auto param){ return  };
        }

    }*/
};

template<typename Fn>
invoking(Fn) ->
    invoking<std::conditional_t<
        std::is_class_v<Fn>,
        std::decay_t<Fn>,
        decltype (std::cref(std::declval<Fn>())) >>;
/*
template <class Fn, class Ret, class... Args>
struct nativeFuncWrapper
{
    using funcType = Fn;
    nativeFuncWrapper(funcType* func) : pFunc(func){
        std::cout<<typeid (*this).name();
    }

    auto operator() (Args&&... args){
        return pFunc(std::forward<Args>(args)...);
    }

    funcType* pFunc;
};

template<class Ret, class... Args>
nativeFuncWrapper(Ret __cdecl (Args...)) ->
    nativeFuncWrapper<Ret __cdecl (Args...),Ret,Args...>;

//todo: create corresponding deductions

template<class Ret, class... Args>
nativeFuncWrapper(Ret __vectorcall (Args...)) ->
    nativeFuncWrapper<Ret __vectorcall (Args...),Ret,Args...>;
    */

//#include <iostream>
/*template <class Fn>
struct nativeFuncWrapper
{
    using funcType = Fn;
    nativeFuncWrapper(funcType func) : pFunc(func){
        //std::cout<<typeid (*this).name();
    }

    template<typename... Args>
    auto operator() (Args&&... args){
        return pFunc(std::forward<Args>(args)...);
    }

    funcType* pFunc;
};

template<typename Fn>
nativeFuncWrapper(Fn) ->
    nativeFuncWrapper<std::remove_pointer_t<std::decay_t<Fn>>>;*/

template<typename Fn>
using wrapped_native_function_t = 
    std::conditional_t<
        std::is_class_v<Fn>, 
        void, 
        decltype(std::cref(std::declval<Fn>()))
    >;


template<typename T>
using inheritable_function =
std::conditional_t<
    std::is_class_v<T>,
    std::decay_t<T>,
    wrapped_native_function_t<T> >;

template <class... Fs>
struct overload : Fs...
{
    overload(Fs&&... fs) : Fs(std::forward<Fs>(fs))... {}

    using Fs::operator()...;
};

/*template<class... Fs>
overload(Fs...) ->
    overload<decltype(std::function(instantiate<Fs>()) )...>;*/
template<class... Fs>
overload(Fs...) ->
    overload<
        inheritable_function<Fs>...
    >;

template <class... Fs>
inline auto overloaded(Fs... fs)
{
    return overload(std::function(fs)...);
}


template <class... Fs>
inline auto functional_overloaded(Fs... fs)
{
    return overload<decltype(std::function(fs))...>(std::function(fs)...);
}

/*
template <class... Fs>
struct overload;

template <class F0, class... Frest>
struct overload<F0, Frest...> : F0, overload<Frest...>
{
    overload(F0 f0, Frest... rest) : F0(f0), overload<Frest...>(rest...) {
        std::cout<<typeid (*this).name();
    }

    using F0::operator();
    using overload<Frest...>::operator();
};

template <class F0>
struct overload<F0> : F0
{
    overload(F0 f0) : F0(f0) { }

    using F0::operator();
};

template<class F0, class... Frest>
overload(F0,Frest...) ->
    overload<decltype(std::function(instantiate<F0>()) ),
             decltype(std::function(instantiate<Frest>()) )...>;

template <class... Fs>
inline auto overloaded(Fs... fs)
{
    return overload<Fs...>(fs...);
}
*/

template<typename Fin, typename... Fs>
#ifdef __cpp_concepts
requires std::conjunction_v<std::is_class<Fin>, std::is_class<Fs>...>
#endif //  __cpp_concepts >= 201907L
struct function_pipe : Fin, function_pipe<Fs...>
{
    using base = function_pipe<Fs...>;
    using self_t = function_pipe<Fin, Fs...>;

    constexpr function_pipe(Fin&& fin, Fs&&... fs) :
        Fin(std::forward<Fin>(fin)),
        base(std::forward<Fs>(fs)...)
    {}

    template<typename... Args>
    auto operator() (Args&&... args) {
        auto ret = Fin::operator() (std::forward<Args>(args)...);
        return base::operator() (std::move(ret));
    }

    template<typename Fo>
    constexpr auto composing(Fo&& fo) {
        return function_pipe<Fin, Fs..., Fo>(
            std::forward<Fin>(*this),
            std::forward<Fs>(*this)...,
            std::forward<Fo>(fo)
        );
    }
};

template<typename Fin>
struct function_pipe<Fin> : Fin
{
    constexpr function_pipe(Fin&& fin) :
        Fin(std::forward<Fin>(fin))
    {}

    template<typename Fo>
    constexpr auto composing(Fo&& fo) {
        return function_pipe<Fin, Fo>(
            std::forward<Fin>(*this),
            std::forward<Fo>(fo)
        );
    }

    using Fin::operator();
};

template<typename Fin, typename... Fs>
    function_pipe(Fin&&, Fs&&...) ->
    function_pipe<inheritable_function<Fin>, inheritable_function<Fs>...>;

template<typename InterType>
struct dynamic_function_pipe
{
    using interfunc_t = std::function<InterType(InterType)>;

    template<typename Fin>
    struct type : Fin
    {
        constexpr type(Fin&& fin) 
            : Fin(std::forward<Fin>(fin))
        {}

        template<typename... Args>
        auto operator() (Args&&... args) {
            auto ret = Fin::operator() (std::forward<Args>(args)...);
            for (auto& f : funcs) {
                ret = f(std::move(ret));
            }
            return std::move(ret);
        }

        template<typename Func>
        void composing(Func&& f) {
            funcs.emplace_back(std::move(f));
        }

        size_t size() const { return funcs.size(); }

        std::vector<interfunc_t> funcs;
    };
};

/// <summary>
/// 带优先级的函数管道
/// 假设std::multimap内部存储的键会被有序遍历
/// </summary>
/// <typeparam name="InterType"></typeparam>
template<typename InterType>
struct priority_function_pipe
{
    using interfunc_t = std::function<InterType(InterType)>;

    template<typename Fin>
    struct type : Fin
    {
        constexpr type(Fin&& fin) 
            : Fin(std::forward<Fin>(fin))
        {}

        template<typename... Args>
        auto operator() (Args&&... args) {
            auto ret = Fin::operator() (std::forward<Args>(args)...);
            for (auto& [priority, f] : funcs) {
                ret = f(std::move(ret));
            }
            return std::move(ret);
        }

        template<typename Func>
        void composing(size_t priority, Func&& f) {
            funcs.emplace(priority, std::move(f));
        }

        size_t size() const { return funcs.size(); }

        std::multimap<size_t, interfunc_t> funcs;
    };
};

template<typename T>
using constified_pointer_t = std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>>;

template<typename T>
inline const T* constify(T* p) { return p; }
template<typename T>
inline const T& constify(T& p) { return p; }

template<typename T>
inline T* deconstify(const T* p) { return const_cast<T*>(p); }
template<typename T>
inline T& deconstify(const T& p) { return const_cast<T&>(p); }


/*
struct property_set_tag {};
struct property_get_tag {};

template<typename FGet, typename FSet>
class property : public FGet, public FSet 
{
public:
    property(property_get_tag, FGet&& fget, property_set_tag, FSet&& fset)
        : FGet(std::forward<FGet>(fget)),
        FSet(std::forward<FSet>(fset))
    { }
    property(property_set_tag, FSet&& fset, property_get_tag, FGet&& fget)
        : FGet(std::forward<FGet>(fget)),
        FSet(std::forward<FSet>(fset))
    { }

    operator decltype(std::declval<FGet>()())() {
        return FGet::operator()();
    }

    template<typename VT>
    auto operator=(VT&& value) {
        return FSet::operator()(std::forward<VT>(value));
    }
};

template<typename FGet, typename FSet>
property(property_set_tag, FSet&&, property_get_tag, FGet&&)->property<FGet, FSet>;

template<typename FGet, typename FSet>
property(property_get_tag, FGet&&, property_set_tag, FSet&&)->property<FGet, FSet>;

#define set property_set_tag(),[]()
#define get property_get_tag(),[](auto value)

struct myClass
{
    int x;
    property myIntProp(
        get { return 1; },
        set {}
    );
};
*/

/*
enum callconv_enum
{
    None,
    cce_cdecl,
    cce_clrcall,
    cce_fastcall,
    cce_stdcall,
    cce_thiscall,
    cce_vectorcall,
};

template<typename T>
struct callconv
{
    constexpr static callconv_enum value = None;
};

template<typename Ret, typename... Args>
struct callconv<Ret __cdecl(Args...)>
{
    constexpr static callconv_enum value = cce_cdecl;
};

template<typename Ret, typename... Args>
struct callconv<Ret __cdecl(Args...)>
{
    constexpr static callconv_enum value = cce_cdecl;
};
*/


#endif // INVOKEFUL_HPP
