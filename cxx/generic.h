#ifndef GENERIC_H
#define GENERIC_H GENERIC_H

#include<iosfwd>
#include<type_traits>

namespace flint {
namespace detail {
struct UNIMPLEMENTED
{
};
}

namespace traits {
template<class T, class U, class Enabler = void>
struct initialization
    : detail::UNIMPLEMENTED
{
    template<class F, class G>
    static void doit(F f, G g) {}
};

template<class T, class U, class Enabler = void>
struct assignment
  : detail::UNIMPLEMENTED
{
    template<class F, class G>
    static void doit(F f, G g) {}
};

template<class T>
struct basic_traits
{
  static const bool is_signed_integer = std::is_signed<T>::value;
  static const bool is_unsigned_integer = std::is_unsigned<T>::value;
};
}

namespace meta {
template<class T>
struct is_implemented
{
private:
    struct no { int data[2];};
    static no n;
    static int test(...) {return 0;}
    static no test(detail::UNIMPLEMENTED) {return n;}

public:
    static const bool res = (sizeof(test(T())) == sizeof(int));
};

template<bool cond, class T = void>
struct enable_if
{
  typedef T res;
};

template<class T>
struct enable_if<false, T>
{
};

template<class T, class Enabler = void>
struct wrap_traits
{
  template<class F, class G>
  static void doit(F, G) {}
};
template<class T>
struct wrap_traits<T, typename enable_if<is_implemented<T>::res>::res>
{
  template<class F, class G>
  static void doit(F&& f, G&& g) {T::doit(f, g);}
};
}

template<class Tag>
class wrapped_object{
public:
    typedef typename Tag::underlying data_t;

private:
    data_t data;

    template<class T>
    static const T& unwrap(const T& t) {return t;}
    template<class T>
    static const typename T::underlying& unwrap(const wrapped_object<T>& t) {return t.get_data();}

    template<class T>
    static void init(const T& t, data_t& data)
    {
        typedef traits::assignment<wrapped_object, T> ass;
        typedef traits::initialization<wrapped_object, T> init;
        static_assert(meta::is_implemented<ass>::res || meta::is_implemented<init>::res,
                      "cannot initilize");
        if(meta::is_implemented<init>::res)
        {
            meta::wrap_traits<init>::doit(data, unwrap(t));
        }
        else
        {
            Tag::empty_initialization(data);
            meta::wrap_traits<ass>::doit(data, unwrap(t));
        }
    }

public:
    wrapped_object()
    {
        Tag::empty_initialization(data);
    }

    wrapped_object(const wrapped_object& o)
    {
        init<wrapped_object>(o, data);
    }

    template<class T>
    explicit wrapped_object(const T& t)
    {
        init<T>(t, data);
    }

    ~wrapped_object()
    {
        Tag::destruction(data);
    }

    void print(std::ostream& o) const
    {
        Tag::print(data, o);
    }

    const data_t& get_data() const {
        return data;
    }
};

template<class T>
std::ostream& operator<<(std::ostream& o, const wrapped_object<T>& w)
{
  w.print(o);
  return o;
}
}

#endif
