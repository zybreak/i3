#ifndef XPP_XINERAMA_HPP
#define XPP_XINERAMA_HPP

#include <string>
#include <vector>

#include <xcb/xinerama.h>

#include "xpp/generic.hpp"

namespace xpp::xinerama {

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
  : public xpp::generic::extension<extension, &xcb_xinerama_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_xinerama_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::xinerama::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::xinerama::event::dispatcher<Connection>;
    using error_dispatcher = xpp::xinerama::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xinerama_query_version_reply), xcb_xinerama_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xinerama_query_version_reply), xcb_xinerama_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xinerama_query_version), xcb_xinerama_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xinerama_query_version_unchecked), xcb_xinerama_query_version_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_version<Connection> query_version(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_version<Connection> query_version_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_version<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_state : public xpp::generic::reply<get_state<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xinerama_get_state_reply), xcb_xinerama_get_state_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_state<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xinerama_get_state_reply), xcb_xinerama_get_state_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_state(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType window(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->window,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_state = detail::get_state<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xinerama_get_state), xcb_xinerama_get_state>>;
}

namespace unchecked {
template<typename Connection>
using get_state = detail::get_state<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xinerama_get_state_unchecked), xcb_xinerama_get_state_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_state<Connection> get_state(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_state<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_state<Connection> get_state_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_state<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_count : public xpp::generic::reply<get_screen_count<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xinerama_get_screen_count_reply), xcb_xinerama_get_screen_count_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_count<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xinerama_get_screen_count_reply), xcb_xinerama_get_screen_count_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_count(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType window(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->window,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_screen_count = detail::get_screen_count<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xinerama_get_screen_count), xcb_xinerama_get_screen_count>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_count = detail::get_screen_count<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xinerama_get_screen_count_unchecked), xcb_xinerama_get_screen_count_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_count<Connection> get_screen_count(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_count<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_count<Connection> get_screen_count_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_count<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_size : public xpp::generic::reply<get_screen_size<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xinerama_get_screen_size_reply), xcb_xinerama_get_screen_size_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_size<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xinerama_get_screen_size_reply), xcb_xinerama_get_screen_size_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_size(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType window(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->window,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_screen_size = detail::get_screen_size<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xinerama_get_screen_size), xcb_xinerama_get_screen_size>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_size = detail::get_screen_size<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xinerama_get_screen_size_unchecked), xcb_xinerama_get_screen_size_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_size<Connection> get_screen_size(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_size<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_size<Connection> get_screen_size_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_size<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class is_active : public xpp::generic::reply<is_active<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xinerama_is_active_reply), xcb_xinerama_is_active_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<is_active<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xinerama_is_active_reply), xcb_xinerama_is_active_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    is_active(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using is_active = detail::is_active<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xinerama_is_active), xcb_xinerama_is_active>>;
}

namespace unchecked {
template<typename Connection>
using is_active = detail::is_active<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xinerama_is_active_unchecked), xcb_xinerama_is_active_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::is_active<Connection> is_active(Connection && c, Parameter && ... parameter) {
  return reply::checked::is_active<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::is_active<Connection> is_active_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::is_active<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_screens : public xpp::generic::reply<query_screens<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xinerama_query_screens_reply), xcb_xinerama_query_screens_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_screens<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xinerama_query_screens_reply), xcb_xinerama_query_screens_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_screens(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xinerama_query_screens_reply_t, xpp::generic::iterator<Connection,
                                              xcb_xinerama_screen_info_t,
                                              xpp::generic::signature<decltype(xcb_xinerama_query_screens_screen_info), xcb_xinerama_query_screens_screen_info>,
                                              xpp::generic::signature<decltype(xcb_xinerama_query_screens_screen_info_length), xcb_xinerama_query_screens_screen_info_length>>>
    screen_info() {
      return xpp::generic::list<Connection,
                                xcb_xinerama_query_screens_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_xinerama_screen_info_t,
                                                       xpp::generic::signature<decltype(xcb_xinerama_query_screens_screen_info), xcb_xinerama_query_screens_screen_info>,
                                                       xpp::generic::signature<decltype(xcb_xinerama_query_screens_screen_info_length), xcb_xinerama_query_screens_screen_info_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_screens = detail::query_screens<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xinerama_query_screens), xcb_xinerama_query_screens>>;
}

namespace unchecked {
template<typename Connection>
using query_screens = detail::query_screens<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xinerama_query_screens_unchecked), xcb_xinerama_query_screens_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_screens<Connection> query_screens(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_screens<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_screens<Connection> query_screens_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_screens<Connection>(
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
    typedef xpp::xinerama::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    xinerama() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::xinerama::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::xinerama::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_state(Parameter && ... parameter) const -> reply::checked::get_state<Connection> {
      return xpp::xinerama::get_state(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_state_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_state<Connection> {
      return xpp::xinerama::get_state_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_count(Parameter && ... parameter) const -> reply::checked::get_screen_count<Connection> {
      return xpp::xinerama::get_screen_count(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_count_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_count<Connection> {
      return xpp::xinerama::get_screen_count_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_size(Parameter && ... parameter) const -> reply::checked::get_screen_size<Connection> {
      return xpp::xinerama::get_screen_size(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_size_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_size<Connection> {
      return xpp::xinerama::get_screen_size_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto is_active(Parameter && ... parameter) const -> reply::checked::is_active<Connection> {
      return xpp::xinerama::is_active(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto is_active_unchecked(Parameter && ... parameter) const -> reply::unchecked::is_active<Connection> {
      return xpp::xinerama::is_active_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_screens(Parameter && ... parameter) const -> reply::checked::query_screens<Connection> {
      return xpp::xinerama::query_screens(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_screens_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_screens<Connection> {
      return xpp::xinerama::query_screens_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::xinerama::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::xinerama::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::xinerama::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::xinerama::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_XINERAMA_HPP
