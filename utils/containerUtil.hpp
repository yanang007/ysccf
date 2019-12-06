#ifndef CONTAINERUTIL_HPP
#define CONTAINERUTIL_HPP

#include <limits>
#include <utility>
#include <tuple>

template <typename _LT,typename _RT>
bool commonLess(const _LT& lhv,const _RT& rhv,signed char sign){
    if( sign > 0 ){
        return lhv < rhv;
    }
    else if ( sign == 0){
        return lhv == rhv;
    }
    else{
        return lhv > rhv;
    }
}

#define TupleOpHelper( FuncName, before, after, cModifier, vModifier) \
    template<class TupType, size_t... I> \
    void _##FuncName##helper(cModifier TupType vModifier _tup, std::index_sequence<I...>) \
    { \
        (..., (before std::get<I>(_tup) after)); \
    } \
    template<class... T> \
    void FuncName (cModifier std::tuple<T...> vModifier _tup) \
    { \
        _##FuncName##helper(_tup, std::make_index_sequence<sizeof...(T)>()); \
    }

#define TupleTransformHelper( FuncName, before, after, cModifier, vModifier) \
    template<class TupType, size_t... I> \
    auto _##FuncName##helper(cModifier TupType vModifier _tup, std::index_sequence<I...>) \
    { \
        return std::make_tuple(before std::get<I>(_tup) after ...); \
    } \
    template<class... T> \
    auto FuncName (cModifier std::tuple<T...> vModifier _tup) \
    { \
        return _##FuncName##helper(_tup, std::make_index_sequence<sizeof...(T)>()); \
    }

/*
template<class TupType, size_t... I>
void print(const TupType& _tup, std::index_sequence<I...>)
{
    (..., (std::cout << (I == 0? "" : ", ") << std::get<I>(_tup)));
    std::cout << ")\n";
}
template<class... T>
void print (const std::tuple<T...>& _tup)
{
    print(_tup, std::make_index_sequence<sizeof...(T)>());
}
*/

template <typename _ContT>
struct reversed
{
    reversed(_ContT& cont):_cont(cont) {}

    auto begin() { return _cont.rbegin(); }
    auto begin() const { return _cont.rbegin(); }
    auto end() { return _cont.rend(); }
    auto end() const { return _cont.rend(); }

    _ContT& _cont;
};

TupleOpHelper(preIncForTuple,++,,,&);
TupleOpHelper(postIncForTuple,,++,,&);
TupleOpHelper(preDecForTuple,--,,,&);
TupleOpHelper(postDecForTuple,,--,,&);
TupleTransformHelper(indirectionForTuple,std::reference_wrapper{*,},,&);

template<class TupType, size_t... I>
bool _tupleAllNotEqualHelper (const TupType& _tupl,const TupType& _tupr, std::index_sequence<I...>)
{
    return ( (std::get<I>(_tupl) != std::get<I>(_tupr)) && ... );
}
template<class... T>
auto tupleAllNotEqual (const std::tuple<T...>& _tupl,decltype(_tupl) _tupr)
{
    return _tupleAllNotEqualHelper(_tupl,_tupr, std::make_index_sequence<sizeof...(T)>());
}

template<class TupType, size_t... I>
bool _tupleAllLessHelper(const TupType& _tupl,const TupType& _tupr, std::index_sequence<I...>)
{
    return ( (std::get<I>(_tupl) < std::get<I>(_tupr)) && ... );
}
template<class... T>
auto tupleAllLess (const std::tuple<T...>& _tupl,decltype(_tupl) _tupr)
{
    return _tupleAllLessHelper(_tupl,_tupr, std::make_index_sequence<sizeof...(T)>());
}

template <typename... _Iters>
struct zippedIteratorEnd : std::tuple<_Iters...>
{
    using base = std::tuple<_Iters...>;
    zippedIteratorEnd(_Iters... iters):base(iters...) {}
};

template <typename... _Iters>
struct zippedIterator : std::tuple<_Iters...>
{
    using base = std::tuple<_Iters...>;
    zippedIterator(_Iters... iters):base(iters...) {}

    auto operator++(){ preIncForTuple(*this); return *this; }
    auto operator++(int){ auto ret(*this);postIncForTuple(*this);return ret; }
    auto operator--(){ preDecForTuple(*this); return *this; }
    auto operator--(int){ auto ret(*this);postDecForTuple(*this);return ret; }

    const auto operator*(){ return indirectionForTuple(*this); }
    const auto operator*() const { return indirectionForTuple(*this); }

    bool operator!=(const zippedIteratorEnd<_Iters...>& rhv) const { return tupleAllNotEqual(*this,rhv); }
};

template<typename... _Iters>
zippedIterator(_Iters...)->
    zippedIterator<_Iters...>;

template <typename _T>
struct range;

template <typename _T>
struct numeralIterator
{
    numeralIterator(const range<_T>& rng):numeralIterator(rng,rng._start) {}
    numeralIterator(const range<_T>& rng,const _T& current):_current(current),_rng(rng) {}

    const auto& operator++(){ _current+=_rng._step; return *this; }
    auto operator++(int){ auto ret = numeralIterator(_rng,_current); _current+=_rng._step; return ret; }
    const auto& operator--(){ _current-=_rng._step; return *this; }
    auto operator--(int){ auto ret = numeralIterator(_rng,_current); _current-=_rng._step; return ret; }

    auto& operator*() { _temp = _current; return _temp; }
    const auto& operator*() const { _temp = _current; return _temp; }

    bool operator<(const numeralIterator<_T>& rhv) const { return commonLess(_current,rhv._current,_rng._step); }
    bool operator==(const numeralIterator<_T>& rhv) const { return commonLess(_current,rhv._current,0); }
    bool operator!=(const numeralIterator<_T>& rhv) const { return !operator==(rhv); }

    _T _temp;
    _T _current;
    const range<_T>& _rng;
};

template <typename _T>
struct range
{
    range(const _T& start,const _T& end,const _T& step):_start(start),_end(end),_step(step) {}
    range(const _T& start,const _T& end):range(start,end,1) {}
    range(const _T& end):range(0,end) {}

    auto begin() { return numeralIterator(*this); }
    auto begin() const { return numeralIterator(*this); }
    auto end() { return numeralIterator(*this,_end); }
    auto end() const { return numeralIterator(*this,_end); }

    _T _start,_end,_step;
};

template <typename _ContT>
struct enumerate
{
    constexpr static size_t maxSize = std::numeric_limits<size_t>::max();
    enumerate(_ContT& cont):_cont(cont),rng(maxSize) {}

    auto begin() { return zippedIterator(rng.begin(),_cont.begin()); }
    auto begin() const { return zippedIterator(rng.begin(),_cont.begin()); }
    auto end() { return zippedIteratorEnd(rng.end(),_cont.end()); }
    auto end() const { return zippedIteratorEnd(rng.end(),_cont.end()); }

    _ContT& _cont;
    range<size_t> rng;
};

#endif // CONTAINERUTIL_HPP
