#ifndef XPP_BIG_REQUESTS_HPP
#define XPP_BIG_REQUESTS_HPP

#include <string>
#include <vector>

#include <xcb/bigreq.h>

#include "xpp/generic.hpp"

namespace xpp::big_requests {

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
  : public xpp::generic::extension<extension, &xcb_big_requests_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_big_requests_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::big_requests::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::big_requests::event::dispatcher<Connection>;
    using error_dispatcher = xpp::big_requests::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class enable : public xpp::generic::reply<enable<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_big_requests_enable_reply), xcb_big_requests_enable_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<enable<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_big_requests_enable_reply), xcb_big_requests_enable_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    enable(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using enable = detail::enable<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_big_requests_enable), xcb_big_requests_enable>>;
}

namespace unchecked {
template<typename Connection>
using enable = detail::enable<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_big_requests_enable_unchecked), xcb_big_requests_enable_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::enable<Connection> enable(Connection && c, Parameter && ... parameter) {
  return reply::checked::enable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::enable<Connection> enable_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::enable<Connection>(
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
    typedef xpp::big_requests::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    big_requests() {
      return *this;
    }

    template<typename ... Parameter>
    auto enable(Parameter && ... parameter) const -> reply::checked::enable<Connection> {
      return xpp::big_requests::enable(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto enable_unchecked(Parameter && ... parameter) const -> reply::unchecked::enable<Connection> {
      return xpp::big_requests::enable_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::big_requests::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::big_requests::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::big_requests::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::big_requests::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_BIG_REQUESTS_HPP
