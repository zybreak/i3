#ifndef XPP_XC_MISC_HPP
#define XPP_XC_MISC_HPP

#include <string>
#include <vector>

#include <xcb/xc_misc.h>

#include "xpp/generic.hpp"

namespace xpp::xc_misc {

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
  : public xpp::generic::extension<extension, &xcb_xc_misc_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_xc_misc_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::xc_misc::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::xc_misc::event::dispatcher<Connection>;
    using error_dispatcher = xpp::xc_misc::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_version : public xpp::generic::reply<get_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xc_misc_get_version_reply), xcb_xc_misc_get_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xc_misc_get_version_reply), xcb_xc_misc_get_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_version = detail::get_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xc_misc_get_version), xcb_xc_misc_get_version>>;
}

namespace unchecked {
template<typename Connection>
using get_version = detail::get_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xc_misc_get_version_unchecked), xcb_xc_misc_get_version_unchecked>>;
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
class get_xid_range : public xpp::generic::reply<get_xid_range<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xc_misc_get_xid_range_reply), xcb_xc_misc_get_xid_range_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_xid_range<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xc_misc_get_xid_range_reply), xcb_xc_misc_get_xid_range_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_xid_range(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_xid_range = detail::get_xid_range<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xc_misc_get_xid_range), xcb_xc_misc_get_xid_range>>;
}

namespace unchecked {
template<typename Connection>
using get_xid_range = detail::get_xid_range<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xc_misc_get_xid_range_unchecked), xcb_xc_misc_get_xid_range_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_xid_range<Connection> get_xid_range(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_xid_range<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_xid_range<Connection> get_xid_range_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_xid_range<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_xid_list : public xpp::generic::reply<get_xid_list<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_reply), xcb_xc_misc_get_xid_list_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_xid_list<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_reply), xcb_xc_misc_get_xid_list_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_xid_list(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xc_misc_get_xid_list_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_ids), xcb_xc_misc_get_xid_list_ids>,
                                              xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_ids_length), xcb_xc_misc_get_xid_list_ids_length>>>
    ids() {
      return xpp::generic::list<Connection,
                                xcb_xc_misc_get_xid_list_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_ids), xcb_xc_misc_get_xid_list_ids>,
                                                       xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_ids_length), xcb_xc_misc_get_xid_list_ids_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_xid_list = detail::get_xid_list<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list), xcb_xc_misc_get_xid_list>>;
}

namespace unchecked {
template<typename Connection>
using get_xid_list = detail::get_xid_list<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xc_misc_get_xid_list_unchecked), xcb_xc_misc_get_xid_list_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_xid_list<Connection> get_xid_list(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_xid_list<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_xid_list<Connection> get_xid_list_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_xid_list<Connection>(
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
    typedef xpp::xc_misc::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    xc_misc() {
      return *this;
    }

    template<typename ... Parameter>
    auto get_version(Parameter && ... parameter) const -> reply::checked::get_version<Connection> {
      return xpp::xc_misc::get_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_version<Connection> {
      return xpp::xc_misc::get_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_xid_range(Parameter && ... parameter) const -> reply::checked::get_xid_range<Connection> {
      return xpp::xc_misc::get_xid_range(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_xid_range_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_xid_range<Connection> {
      return xpp::xc_misc::get_xid_range_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_xid_list(Parameter && ... parameter) const -> reply::checked::get_xid_list<Connection> {
      return xpp::xc_misc::get_xid_list(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_xid_list_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_xid_list<Connection> {
      return xpp::xc_misc::get_xid_list_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::xc_misc::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::xc_misc::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::xc_misc::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::xc_misc::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_XC_MISC_HPP
