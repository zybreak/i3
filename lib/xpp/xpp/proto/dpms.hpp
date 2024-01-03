#ifndef XPP_DPMS_HPP
#define XPP_DPMS_HPP

#include <string>
#include <vector>

#include <xcb/dpms.h>

#include "xpp/generic.hpp"

namespace xpp::dpms {

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
  : public xpp::generic::extension<extension, &xcb_dpms_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_dpms_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::dpms::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::dpms::event::dispatcher<Connection>;
    using error_dispatcher = xpp::dpms::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_version : public xpp::generic::reply<get_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_dpms_get_version_reply), xcb_dpms_get_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_dpms_get_version_reply), xcb_dpms_get_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_version = detail::get_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_dpms_get_version), xcb_dpms_get_version>>;
}

namespace unchecked {
template<typename Connection>
using get_version = detail::get_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_dpms_get_version_unchecked), xcb_dpms_get_version_unchecked>>;
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
class capable : public xpp::generic::reply<capable<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_dpms_capable_reply), xcb_dpms_capable_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<capable<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_dpms_capable_reply), xcb_dpms_capable_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    capable(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using capable = detail::capable<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_dpms_capable), xcb_dpms_capable>>;
}

namespace unchecked {
template<typename Connection>
using capable = detail::capable<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_dpms_capable_unchecked), xcb_dpms_capable_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::capable<Connection> capable(Connection && c, Parameter && ... parameter) {
  return reply::checked::capable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::capable<Connection> capable_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::capable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_timeouts : public xpp::generic::reply<get_timeouts<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_dpms_get_timeouts_reply), xcb_dpms_get_timeouts_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_timeouts<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_dpms_get_timeouts_reply), xcb_dpms_get_timeouts_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_timeouts(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_timeouts = detail::get_timeouts<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_dpms_get_timeouts), xcb_dpms_get_timeouts>>;
}

namespace unchecked {
template<typename Connection>
using get_timeouts = detail::get_timeouts<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_dpms_get_timeouts_unchecked), xcb_dpms_get_timeouts_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_timeouts<Connection> get_timeouts(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_timeouts<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_timeouts<Connection> get_timeouts_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_timeouts<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_timeouts_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::dpms::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_dpms_set_timeouts_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_timeouts(Parameter && ... parameter) {
  xcb_dpms_set_timeouts(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void enable_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::dpms::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_dpms_enable_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void enable(Parameter && ... parameter) {
  xcb_dpms_enable(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void disable_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::dpms::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_dpms_disable_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void disable(Parameter && ... parameter) {
  xcb_dpms_disable(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void force_level_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::dpms::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_dpms_force_level_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void force_level(Parameter && ... parameter) {
  xcb_dpms_force_level(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class info : public xpp::generic::reply<info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_dpms_info_reply), xcb_dpms_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_dpms_info_reply), xcb_dpms_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using info = detail::info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_dpms_info), xcb_dpms_info>>;
}

namespace unchecked {
template<typename Connection>
using info = detail::info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_dpms_info_unchecked), xcb_dpms_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::info<Connection> info(Connection && c, Parameter && ... parameter) {
  return reply::checked::info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::info<Connection> info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}



































template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::dpms::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    dpms() {
      return *this;
    }

    template<typename ... Parameter>
    auto get_version(Parameter && ... parameter) const -> reply::checked::get_version<Connection> {
      return xpp::dpms::get_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_version<Connection> {
      return xpp::dpms::get_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto capable(Parameter && ... parameter) const -> reply::checked::capable<Connection> {
      return xpp::dpms::capable(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto capable_unchecked(Parameter && ... parameter) const -> reply::unchecked::capable<Connection> {
      return xpp::dpms::capable_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_timeouts(Parameter && ... parameter) const -> reply::checked::get_timeouts<Connection> {
      return xpp::dpms::get_timeouts(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_timeouts_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_timeouts<Connection> {
      return xpp::dpms::get_timeouts_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_timeouts_checked(Parameter && ... parameter) const {
      xpp::dpms::set_timeouts_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_timeouts(Parameter && ... parameter) const {
      xpp::dpms::set_timeouts(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void enable_checked(Parameter && ... parameter) const {
      xpp::dpms::enable_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void enable(Parameter && ... parameter) const {
      xpp::dpms::enable(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void disable_checked(Parameter && ... parameter) const {
      xpp::dpms::disable_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void disable(Parameter && ... parameter) const {
      xpp::dpms::disable(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void force_level_checked(Parameter && ... parameter) const {
      xpp::dpms::force_level_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void force_level(Parameter && ... parameter) const {
      xpp::dpms::force_level(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto info(Parameter && ... parameter) const -> reply::checked::info<Connection> {
      return xpp::dpms::info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto info_unchecked(Parameter && ... parameter) const -> reply::unchecked::info<Connection> {
      return xpp::dpms::info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::dpms::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::dpms::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::dpms::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::dpms::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_DPMS_HPP
