#ifndef XPP_XEVIE_HPP
#define XPP_XEVIE_HPP

#include <string>
#include <vector>

#include <xcb/xevie.h>

#include "xpp/generic.hpp"

namespace xpp::xevie {

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
  : public xpp::generic::extension<extension, &xcb_xevie_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_xevie_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::xevie::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::xevie::event::dispatcher<Connection>;
    using error_dispatcher = xpp::xevie::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xevie_query_version_reply), xcb_xevie_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xevie_query_version_reply), xcb_xevie_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xevie_query_version), xcb_xevie_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xevie_query_version_unchecked), xcb_xevie_query_version_unchecked>>;
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
class start : public xpp::generic::reply<start<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xevie_start_reply), xcb_xevie_start_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<start<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xevie_start_reply), xcb_xevie_start_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    start(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using start = detail::start<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xevie_start), xcb_xevie_start>>;
}

namespace unchecked {
template<typename Connection>
using start = detail::start<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xevie_start_unchecked), xcb_xevie_start_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::start<Connection> start(Connection && c, Parameter && ... parameter) {
  return reply::checked::start<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::start<Connection> start_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::start<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class end : public xpp::generic::reply<end<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xevie_end_reply), xcb_xevie_end_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<end<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xevie_end_reply), xcb_xevie_end_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    end(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using end = detail::end<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xevie_end), xcb_xevie_end>>;
}

namespace unchecked {
template<typename Connection>
using end = detail::end<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xevie_end_unchecked), xcb_xevie_end_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::end<Connection> end(Connection && c, Parameter && ... parameter) {
  return reply::checked::end<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::end<Connection> end_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::end<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class send : public xpp::generic::reply<send<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xevie_send_reply), xcb_xevie_send_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<send<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xevie_send_reply), xcb_xevie_send_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    send(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using send = detail::send<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xevie_send), xcb_xevie_send>>;
}

namespace unchecked {
template<typename Connection>
using send = detail::send<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xevie_send_unchecked), xcb_xevie_send_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::send<Connection> send(Connection && c, Parameter && ... parameter) {
  return reply::checked::send<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::send<Connection> send_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::send<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class select_input : public xpp::generic::reply<select_input<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xevie_select_input_reply), xcb_xevie_select_input_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<select_input<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xevie_select_input_reply), xcb_xevie_select_input_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    select_input(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using select_input = detail::select_input<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xevie_select_input), xcb_xevie_select_input>>;
}

namespace unchecked {
template<typename Connection>
using select_input = detail::select_input<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xevie_select_input_unchecked), xcb_xevie_select_input_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::select_input<Connection> select_input(Connection && c, Parameter && ... parameter) {
  return reply::checked::select_input<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::select_input<Connection> select_input_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::select_input<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}



template<typename Derived, typename Connection>
class event
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_xevie_event_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~event() = default;

    template<typename ... Parameter>
    auto send(Parameter && ... parameter) const -> reply::checked::send<Connection> {
      return xpp::xevie::send(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto send_unchecked(Parameter && ... parameter) const -> reply::unchecked::send<Connection> {
      return xpp::xevie::send_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};
































template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::xevie::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    xevie() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::xevie::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::xevie::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto start(Parameter && ... parameter) const -> reply::checked::start<Connection> {
      return xpp::xevie::start(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto start_unchecked(Parameter && ... parameter) const -> reply::unchecked::start<Connection> {
      return xpp::xevie::start_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto end(Parameter && ... parameter) const -> reply::checked::end<Connection> {
      return xpp::xevie::end(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto end_unchecked(Parameter && ... parameter) const -> reply::unchecked::end<Connection> {
      return xpp::xevie::end_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto send(Parameter && ... parameter) const -> reply::checked::send<Connection> {
      return xpp::xevie::send(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto send_unchecked(Parameter && ... parameter) const -> reply::unchecked::send<Connection> {
      return xpp::xevie::send_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto select_input(Parameter && ... parameter) const -> reply::checked::select_input<Connection> {
      return xpp::xevie::select_input(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto select_input_unchecked(Parameter && ... parameter) const -> reply::unchecked::select_input<Connection> {
      return xpp::xevie::select_input_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::xevie::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::xevie::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::xevie::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::xevie::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_XEVIE_HPP
