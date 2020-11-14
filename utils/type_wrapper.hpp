#ifndef TYPE_WRAPPER_HPP
#define TYPE_WRAPPER_HPP

template<typename Type, size_t id>
struct type_wrapper
{
    type_wrapper() = default;
    type_wrapper(const Type& v) : value(v) {}
    type_wrapper(Type&& v) : value(std::move(v)) {}

    Type& get() { return value; }
    const Type& get() const { return value; }

    operator Type& () { return get(); }
    operator const Type& () const { return get(); }

    Type value;
};

#endif //TYPE_WRAPPER_HPP
