#ifndef INVOKEFUL_HPP
#define INVOKEFUL_HPP

#include <tuple>
#include <functional>

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
auto decay_forward(_T arg)
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
        decltype (nativeFuncWrapper(std::declval<Fn>())) >>;
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
template <class Fn>
struct nativeFuncWrapper
{
    using funcType = Fn;
    nativeFuncWrapper(funcType* func) : pFunc(func){
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
    nativeFuncWrapper<std::decay_t<Fn> >;

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
        std::conditional_t<
            std::is_class_v<Fs>,
            std::decay_t<Fs>,
            decltype (nativeFuncWrapper(std::declval<Fs>())) > ...>;

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

#endif // INVOKEFUL_HPP
