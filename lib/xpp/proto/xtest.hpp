#ifndef XPP_TEST_HPP
#define XPP_TEST_HPP

#include <string>
#include <vector>

#include <xcb/xtest.h>

#include "xpp/generic.hpp"

namespace xpp::test {

template<typename Derived, typename Connection>
class interface;

namespace event {
template<typename Connection>
class dispatcher;
}

namespace error {
class dispatcher;
}

class extension
  : public xpp::generic::extension<extension, &xcb_test_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_test_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::test::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::test::event::dispatcher<Connection>;
    using error_dispatcher = xpp::test::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_version : public xpp::generic::reply<get_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_test_get_version_reply), xcb_test_get_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_test_get_version_reply), xcb_test_get_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_version = detail::get_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_test_get_version), xcb_test_get_version>>;
}

namespace unchecked {
template<typename Connection>
using get_version = detail::get_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_test_get_version_unchecked), xcb_test_get_version_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_version<Connection> get_version(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_version<Connection> get_version_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class compare_cursor : public xpp::generic::reply<compare_cursor<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_test_compare_cursor_reply), xcb_test_compare_cursor_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<compare_cursor<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_test_compare_cursor_reply), xcb_test_compare_cursor_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    compare_cursor(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using compare_cursor = detail::compare_cursor<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_test_compare_cursor), xcb_test_compare_cursor>>;
}

namespace unchecked {
template<typename Connection>
using compare_cursor = detail::compare_cursor<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_test_compare_cursor_unchecked), xcb_test_compare_cursor_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::compare_cursor<Connection> compare_cursor(Connection && c, Parameter && ... parameter) {
  return reply::checked::compare_cursor<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::compare_cursor<Connection> compare_cursor_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::compare_cursor<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void fake_input_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::test::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_test_fake_input_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void fake_input(Parameter && ... parameter) {
  xcb_test_fake_input(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void grab_control_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::test::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_test_grab_control_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void grab_control(Parameter && ... parameter) {
  xcb_test_grab_control(std::forward<Parameter>(parameter) ...);
}



































template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::test::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    test() {
      return *this;
    }

    template<typename ... Parameter>
    auto get_version(Parameter && ... parameter) const -> reply::checked::get_version<Connection> {
      return xpp::test::get_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_version<Connection> {
      return xpp::test::get_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto compare_cursor(Parameter && ... parameter) const -> reply::checked::compare_cursor<Connection> {
      return xpp::test::compare_cursor(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto compare_cursor_unchecked(Parameter && ... parameter) const -> reply::unchecked::compare_cursor<Connection> {
      return xpp::test::compare_cursor_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void fake_input_checked(Parameter && ... parameter) const {
      xpp::test::fake_input_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void fake_input(Parameter && ... parameter) const {
      xpp::test::fake_input(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void grab_control_checked(Parameter && ... parameter) const {
      xpp::test::grab_control_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void grab_control(Parameter && ... parameter) const {
      xpp::test::grab_control(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::test::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::test::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler, const std::shared_ptr<xcb_generic_event_t> &) const {
      return false;
    }

  protected:
    Connection m_c;
    uint8_t m_first_event;
};

}

namespace error {

class dispatcher {
  public:
    typedef xpp::test::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::test::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_TEST_HPP
