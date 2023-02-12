#ifndef XPP_GENERIC_INPUT_ITERATOR_ADAPTER_HPP
#define XPP_GENERIC_INPUT_ITERATOR_ADAPTER_HPP

#include <utility>
#include <iterator>
#include <type_traits>

template<typename T, bool B>
class HasMember_first {
private:
    using Yes = char[2];
    using No = char[1];
    struct Fallback {
        int first;
    };
    struct Derived : T, Fallback {
    };

    template<typename U>
    static No &test(decltype(U::first) *);

    template<typename U>
    static Yes &test(U *);

public:
    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes);
};

template<typename T>
class HasMember_first<T, false> {
public:
    static constexpr bool RESULT = false;
};

template<typename T>
struct has_member_first : public std::integral_constant<bool, HasMember_first<T, std::is_class<T>::value>::RESULT> {
};

template<typename T, bool B>
class HasMember_second {
private:
    using Yes = char[2];
    using No = char[1];
    struct Fallback {
        int second;
    };
    struct Derived : T, Fallback {
    };

    template<typename U>
    static No &test(decltype(U::second) *);

    template<typename U>
    static Yes &test(U *);

public:
    static constexpr bool RESULT = sizeof(test<Derived>(nullptr)) == sizeof(Yes);
};

template<typename T>
class HasMember_second<T, false> {
public:
    static constexpr bool RESULT = false;
};

template<typename T>
struct has_member_second : public std::integral_constant<bool, HasMember_second<T, std::is_class<T>::value>::RESULT> {
};

// namespace iterator {

template<typename Iterator>
struct value_iterator_base {
    value_iterator_base(const Iterator &iterator) : m_iterator(iterator) {
    }

    bool operator==(const value_iterator_base &other) {
        return m_iterator == other.m_iterator;
    }

    bool operator!=(const value_iterator_base &other) {
        return m_iterator != other.m_iterator;
    }

    void operator++() {
        ++m_iterator;
    }

    template<typename Key, typename Value>
    const Value &get_value(const std::pair<Key, Value> &pair) {
        return pair.second;
    }

    template<typename Value>
    const Value &get_value(const Value &v) {
        return v;
    }

    Iterator m_iterator;
};

template<typename Iterator>
struct value_iterator_pair : public value_iterator_base<Iterator> {
    using iterator_category = typename std::input_iterator_tag;
    using value_type = typename Iterator::value_type::second_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = typename Iterator::value_type::second_type *;
    using reference = const typename Iterator::value_type::second_type &;

    typedef value_iterator_base<Iterator> base;
    using base::base;

    const typename Iterator::value_type::second_type &operator*() {
        return base::get_value(*base::m_iterator);
    }
};

template<typename Iterator>
struct value_iterator_integral : public value_iterator_base<Iterator> {
    using iterator_category = typename std::input_iterator_tag;
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using reference = typename std::iterator_traits<Iterator>::reference;

    typedef value_iterator_base<Iterator> base;
    using base::base;

    const typename Iterator::value_type &operator*() {
        return base::get_value(*base::m_iterator);
    }
};

template<typename Iterator>
struct value_iterator : public std::conditional<has_member_first<typename Iterator::value_type>::value && has_member_second<typename Iterator::value_type>::value, value_iterator_pair<Iterator>, value_iterator_integral<Iterator> >::type {
    typedef typename std::conditional<has_member_first<typename Iterator::value_type>::value && has_member_second<typename Iterator::value_type>::value, value_iterator_pair<Iterator>, value_iterator_integral<Iterator> >::type base;
    using base::base;
};

template<typename T, bool B = true>
struct value_type {
    typedef typename std::conditional<has_member_second<typename T::value_type>::value, typename T::value_type::second_type, typename T::value_type>::type type;
};

template<typename T>
struct value_type<T, false> {
    typedef typename std::remove_const<typename std::remove_pointer<T>::type>::type type;
};

#endif // XPP_GENERIC_INPUT_ITERATOR_ADAPTER_HPP
