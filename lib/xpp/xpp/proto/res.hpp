#ifndef XPP_RES_HPP
#define XPP_RES_HPP

#include <string>
#include <vector>

#include <xcb/res.h>

#include "xpp/generic.hpp"

namespace xpp::res {

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
  : public xpp::generic::extension<extension, &xcb_res_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_res_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::res::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::res::event::dispatcher<Connection>;
    using error_dispatcher = xpp::res::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_res_query_version_reply), xcb_res_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_res_query_version_reply), xcb_res_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_res_query_version), xcb_res_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_res_query_version_unchecked), xcb_res_query_version_unchecked>>;
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
class query_clients : public xpp::generic::reply<query_clients<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_res_query_clients_reply), xcb_res_query_clients_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_clients<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_res_query_clients_reply), xcb_res_query_clients_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_clients(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_res_query_clients_reply_t, xpp::generic::iterator<Connection,
                                              xcb_res_client_t,
                                              xpp::generic::signature<decltype(xcb_res_query_clients_clients), xcb_res_query_clients_clients>,
                                              xpp::generic::signature<decltype(xcb_res_query_clients_clients_length), xcb_res_query_clients_clients_length>>>
    clients() {
      return xpp::generic::list<Connection,
                                xcb_res_query_clients_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_res_client_t,
                                                       xpp::generic::signature<decltype(xcb_res_query_clients_clients), xcb_res_query_clients_clients>,
                                                       xpp::generic::signature<decltype(xcb_res_query_clients_clients_length), xcb_res_query_clients_clients_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_clients = detail::query_clients<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_res_query_clients), xcb_res_query_clients>>;
}

namespace unchecked {
template<typename Connection>
using query_clients = detail::query_clients<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_res_query_clients_unchecked), xcb_res_query_clients_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_clients<Connection> query_clients(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_clients<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_clients<Connection> query_clients_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_clients<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_client_resources : public xpp::generic::reply<query_client_resources<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_res_query_client_resources_reply), xcb_res_query_client_resources_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_client_resources<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_res_query_client_resources_reply), xcb_res_query_client_resources_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_client_resources(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_res_query_client_resources_reply_t, xpp::generic::iterator<Connection,
                                              xcb_res_type_t,
                                              xpp::generic::signature<decltype(xcb_res_query_client_resources_types), xcb_res_query_client_resources_types>,
                                              xpp::generic::signature<decltype(xcb_res_query_client_resources_types_length), xcb_res_query_client_resources_types_length>>>
    types() {
      return xpp::generic::list<Connection,
                                xcb_res_query_client_resources_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_res_type_t,
                                                       xpp::generic::signature<decltype(xcb_res_query_client_resources_types), xcb_res_query_client_resources_types>,
                                                       xpp::generic::signature<decltype(xcb_res_query_client_resources_types_length), xcb_res_query_client_resources_types_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_client_resources = detail::query_client_resources<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_res_query_client_resources), xcb_res_query_client_resources>>;
}

namespace unchecked {
template<typename Connection>
using query_client_resources = detail::query_client_resources<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_res_query_client_resources_unchecked), xcb_res_query_client_resources_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_client_resources<Connection> query_client_resources(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_client_resources<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_client_resources<Connection> query_client_resources_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_client_resources<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_client_pixmap_bytes : public xpp::generic::reply<query_client_pixmap_bytes<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_res_query_client_pixmap_bytes_reply), xcb_res_query_client_pixmap_bytes_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_client_pixmap_bytes<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_res_query_client_pixmap_bytes_reply), xcb_res_query_client_pixmap_bytes_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_client_pixmap_bytes(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_client_pixmap_bytes = detail::query_client_pixmap_bytes<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_res_query_client_pixmap_bytes), xcb_res_query_client_pixmap_bytes>>;
}

namespace unchecked {
template<typename Connection>
using query_client_pixmap_bytes = detail::query_client_pixmap_bytes<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_res_query_client_pixmap_bytes_unchecked), xcb_res_query_client_pixmap_bytes_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_client_pixmap_bytes<Connection> query_client_pixmap_bytes(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_client_pixmap_bytes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_client_pixmap_bytes<Connection> query_client_pixmap_bytes_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_client_pixmap_bytes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_client_ids : public xpp::generic::reply<query_client_ids<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_res_query_client_ids_reply), xcb_res_query_client_ids_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_client_ids<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_res_query_client_ids_reply), xcb_res_query_client_ids_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_client_ids(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_res_query_client_ids_reply_t, xpp::generic::iterator<Connection,
                                              xcb_res_client_id_value_t,
                                              xpp::generic::signature<decltype(xcb_res_client_id_value_next), xcb_res_client_id_value_next>,
                                              xpp::generic::signature<decltype(xcb_res_client_id_value_sizeof), xcb_res_client_id_value_sizeof>,
                                              xpp::generic::signature<decltype(xcb_res_query_client_ids_ids_iterator), xcb_res_query_client_ids_ids_iterator>>>
    ids() {
      return xpp::generic::list<Connection,
                                xcb_res_query_client_ids_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_res_client_id_value_t,
                                                       xpp::generic::signature<decltype(xcb_res_client_id_value_next), xcb_res_client_id_value_next>,
                                                       xpp::generic::signature<decltype(xcb_res_client_id_value_sizeof), xcb_res_client_id_value_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_res_query_client_ids_ids_iterator), xcb_res_query_client_ids_ids_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_client_ids = detail::query_client_ids<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_res_query_client_ids), xcb_res_query_client_ids>>;
}

namespace unchecked {
template<typename Connection>
using query_client_ids = detail::query_client_ids<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_res_query_client_ids_unchecked), xcb_res_query_client_ids_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_client_ids<Connection> query_client_ids(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_client_ids<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_client_ids<Connection> query_client_ids_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_client_ids<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_resource_bytes : public xpp::generic::reply<query_resource_bytes<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_res_query_resource_bytes_reply), xcb_res_query_resource_bytes_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_resource_bytes<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_res_query_resource_bytes_reply), xcb_res_query_resource_bytes_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_resource_bytes(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_res_query_resource_bytes_reply_t, xpp::generic::iterator<Connection,
                                              xcb_res_resource_size_value_t,
                                              xpp::generic::signature<decltype(xcb_res_resource_size_value_next), xcb_res_resource_size_value_next>,
                                              xpp::generic::signature<decltype(xcb_res_resource_size_value_sizeof), xcb_res_resource_size_value_sizeof>,
                                              xpp::generic::signature<decltype(xcb_res_query_resource_bytes_sizes_iterator), xcb_res_query_resource_bytes_sizes_iterator>>>
    sizes() {
      return xpp::generic::list<Connection,
                                xcb_res_query_resource_bytes_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_res_resource_size_value_t,
                                                       xpp::generic::signature<decltype(xcb_res_resource_size_value_next), xcb_res_resource_size_value_next>,
                                                       xpp::generic::signature<decltype(xcb_res_resource_size_value_sizeof), xcb_res_resource_size_value_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_res_query_resource_bytes_sizes_iterator), xcb_res_query_resource_bytes_sizes_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_resource_bytes = detail::query_resource_bytes<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_res_query_resource_bytes), xcb_res_query_resource_bytes>>;
}

namespace unchecked {
template<typename Connection>
using query_resource_bytes = detail::query_resource_bytes<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_res_query_resource_bytes_unchecked), xcb_res_query_resource_bytes_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_resource_bytes<Connection> query_resource_bytes(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_resource_bytes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_resource_bytes<Connection> query_resource_bytes_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_resource_bytes<Connection>(
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
    typedef xpp::res::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    res() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::res::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::res::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_clients(Parameter && ... parameter) const -> reply::checked::query_clients<Connection> {
      return xpp::res::query_clients(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_clients_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_clients<Connection> {
      return xpp::res::query_clients_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_client_resources(Parameter && ... parameter) const -> reply::checked::query_client_resources<Connection> {
      return xpp::res::query_client_resources(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_client_resources_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_client_resources<Connection> {
      return xpp::res::query_client_resources_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_client_pixmap_bytes(Parameter && ... parameter) const -> reply::checked::query_client_pixmap_bytes<Connection> {
      return xpp::res::query_client_pixmap_bytes(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_client_pixmap_bytes_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_client_pixmap_bytes<Connection> {
      return xpp::res::query_client_pixmap_bytes_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_client_ids(Parameter && ... parameter) const -> reply::checked::query_client_ids<Connection> {
      return xpp::res::query_client_ids(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_client_ids_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_client_ids<Connection> {
      return xpp::res::query_client_ids_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_resource_bytes(Parameter && ... parameter) const -> reply::checked::query_resource_bytes<Connection> {
      return xpp::res::query_resource_bytes(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_resource_bytes_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_resource_bytes<Connection> {
      return xpp::res::query_resource_bytes_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::res::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::res::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::res::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::res::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_RES_HPP
