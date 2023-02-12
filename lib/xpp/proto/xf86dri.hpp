#ifndef XPP_XF86DRI_HPP
#define XPP_XF86DRI_HPP

#include <string>
#include <vector>

#include <xcb/xf86dri.h>

#include "xpp/generic.hpp"

namespace xpp::xf86dri {

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
  : public xpp::generic::extension<extension, &xcb_xf86dri_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_xf86dri_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::xf86dri::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::xf86dri::event::dispatcher<Connection>;
    using error_dispatcher = xpp::xf86dri::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_query_version_reply), xcb_xf86dri_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_query_version_reply), xcb_xf86dri_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_query_version), xcb_xf86dri_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_query_version_unchecked), xcb_xf86dri_query_version_unchecked>>;
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
class query_direct_rendering_capable : public xpp::generic::reply<query_direct_rendering_capable<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_query_direct_rendering_capable_reply), xcb_xf86dri_query_direct_rendering_capable_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_direct_rendering_capable<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_query_direct_rendering_capable_reply), xcb_xf86dri_query_direct_rendering_capable_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_direct_rendering_capable(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_direct_rendering_capable = detail::query_direct_rendering_capable<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_query_direct_rendering_capable), xcb_xf86dri_query_direct_rendering_capable>>;
}

namespace unchecked {
template<typename Connection>
using query_direct_rendering_capable = detail::query_direct_rendering_capable<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_query_direct_rendering_capable_unchecked), xcb_xf86dri_query_direct_rendering_capable_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_direct_rendering_capable<Connection> query_direct_rendering_capable(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_direct_rendering_capable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_direct_rendering_capable<Connection> query_direct_rendering_capable_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_direct_rendering_capable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class open_connection : public xpp::generic::reply<open_connection<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_open_connection_reply), xcb_xf86dri_open_connection_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<open_connection<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_open_connection_reply), xcb_xf86dri_open_connection_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    open_connection(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    std::string bus_id() {
      return std::string(xcb_xf86dri_open_connection_bus_id(this->get().get()), xcb_xf86dri_open_connection_bus_id_length(this->get().get()));
    }

};

}

namespace checked {
template<typename Connection>
using open_connection = detail::open_connection<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_open_connection), xcb_xf86dri_open_connection>>;
}

namespace unchecked {
template<typename Connection>
using open_connection = detail::open_connection<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_open_connection_unchecked), xcb_xf86dri_open_connection_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::open_connection<Connection> open_connection(Connection && c, Parameter && ... parameter) {
  return reply::checked::open_connection<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::open_connection<Connection> open_connection_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::open_connection<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void close_connection_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::xf86dri::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_xf86dri_close_connection_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void close_connection(Parameter && ... parameter) {
  xcb_xf86dri_close_connection(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_client_driver_name : public xpp::generic::reply<get_client_driver_name<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_get_client_driver_name_reply), xcb_xf86dri_get_client_driver_name_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_client_driver_name<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_get_client_driver_name_reply), xcb_xf86dri_get_client_driver_name_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_client_driver_name(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    std::string client_driver_name() {
      return std::string(xcb_xf86dri_get_client_driver_name_client_driver_name(this->get().get()), xcb_xf86dri_get_client_driver_name_client_driver_name_length(this->get().get()));
    }

};

}

namespace checked {
template<typename Connection>
using get_client_driver_name = detail::get_client_driver_name<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_get_client_driver_name), xcb_xf86dri_get_client_driver_name>>;
}

namespace unchecked {
template<typename Connection>
using get_client_driver_name = detail::get_client_driver_name<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_get_client_driver_name_unchecked), xcb_xf86dri_get_client_driver_name_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_client_driver_name<Connection> get_client_driver_name(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_client_driver_name<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_client_driver_name<Connection> get_client_driver_name_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_client_driver_name<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_context : public xpp::generic::reply<create_context<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_create_context_reply), xcb_xf86dri_create_context_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_context<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_create_context_reply), xcb_xf86dri_create_context_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_context(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using create_context = detail::create_context<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_create_context), xcb_xf86dri_create_context>>;
}

namespace unchecked {
template<typename Connection>
using create_context = detail::create_context<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_create_context_unchecked), xcb_xf86dri_create_context_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_context<Connection> create_context(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_context<Connection> create_context_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_context<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_context_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::xf86dri::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_xf86dri_destroy_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_context(Parameter && ... parameter) {
  xcb_xf86dri_destroy_context(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_drawable : public xpp::generic::reply<create_drawable<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_create_drawable_reply), xcb_xf86dri_create_drawable_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_drawable<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_create_drawable_reply), xcb_xf86dri_create_drawable_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_drawable(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using create_drawable = detail::create_drawable<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_create_drawable), xcb_xf86dri_create_drawable>>;
}

namespace unchecked {
template<typename Connection>
using create_drawable = detail::create_drawable<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_create_drawable_unchecked), xcb_xf86dri_create_drawable_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_drawable<Connection> create_drawable(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_drawable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_drawable<Connection> create_drawable_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_drawable<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_drawable_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::xf86dri::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_xf86dri_destroy_drawable_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_drawable(Parameter && ... parameter) {
  xcb_xf86dri_destroy_drawable(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_drawable_info : public xpp::generic::reply<get_drawable_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_reply), xcb_xf86dri_get_drawable_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_drawable_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_reply), xcb_xf86dri_get_drawable_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_drawable_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xf86dri_get_drawable_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_xf86dri_drm_clip_rect_t,
                                              xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_clip_rects), xcb_xf86dri_get_drawable_info_clip_rects>,
                                              xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_clip_rects_length), xcb_xf86dri_get_drawable_info_clip_rects_length>>>
    clip_rects() {
      return xpp::generic::list<Connection,
                                xcb_xf86dri_get_drawable_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_xf86dri_drm_clip_rect_t,
                                                       xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_clip_rects), xcb_xf86dri_get_drawable_info_clip_rects>,
                                                       xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_clip_rects_length), xcb_xf86dri_get_drawable_info_clip_rects_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_xf86dri_get_drawable_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_xf86dri_drm_clip_rect_t,
                                              xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_back_clip_rects), xcb_xf86dri_get_drawable_info_back_clip_rects>,
                                              xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_back_clip_rects_length), xcb_xf86dri_get_drawable_info_back_clip_rects_length>>>
    back_clip_rects() {
      return xpp::generic::list<Connection,
                                xcb_xf86dri_get_drawable_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_xf86dri_drm_clip_rect_t,
                                                       xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_back_clip_rects), xcb_xf86dri_get_drawable_info_back_clip_rects>,
                                                       xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_back_clip_rects_length), xcb_xf86dri_get_drawable_info_back_clip_rects_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_drawable_info = detail::get_drawable_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info), xcb_xf86dri_get_drawable_info>>;
}

namespace unchecked {
template<typename Connection>
using get_drawable_info = detail::get_drawable_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_get_drawable_info_unchecked), xcb_xf86dri_get_drawable_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_drawable_info<Connection> get_drawable_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_drawable_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_drawable_info<Connection> get_drawable_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_drawable_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_device_info : public xpp::generic::reply<get_device_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_reply), xcb_xf86dri_get_device_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_device_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_reply), xcb_xf86dri_get_device_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_device_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xf86dri_get_device_info_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_device_private), xcb_xf86dri_get_device_info_device_private>,
                                              xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_device_private_length), xcb_xf86dri_get_device_info_device_private_length>>>
    device_private() {
      return xpp::generic::list<Connection,
                                xcb_xf86dri_get_device_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_device_private), xcb_xf86dri_get_device_info_device_private>,
                                                       xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_device_private_length), xcb_xf86dri_get_device_info_device_private_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_device_info = detail::get_device_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_get_device_info), xcb_xf86dri_get_device_info>>;
}

namespace unchecked {
template<typename Connection>
using get_device_info = detail::get_device_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_get_device_info_unchecked), xcb_xf86dri_get_device_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_device_info<Connection> get_device_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_device_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_device_info<Connection> get_device_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_device_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class auth_connection : public xpp::generic::reply<auth_connection<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xf86dri_auth_connection_reply), xcb_xf86dri_auth_connection_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<auth_connection<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xf86dri_auth_connection_reply), xcb_xf86dri_auth_connection_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    auth_connection(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using auth_connection = detail::auth_connection<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xf86dri_auth_connection), xcb_xf86dri_auth_connection>>;
}

namespace unchecked {
template<typename Connection>
using auth_connection = detail::auth_connection<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xf86dri_auth_connection_unchecked), xcb_xf86dri_auth_connection_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::auth_connection<Connection> auth_connection(Connection && c, Parameter && ... parameter) {
  return reply::checked::auth_connection<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::auth_connection<Connection> auth_connection_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::auth_connection<Connection>(
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
    typedef xpp::xf86dri::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    xf86dri() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::xf86dri::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::xf86dri::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_direct_rendering_capable(Parameter && ... parameter) const -> reply::checked::query_direct_rendering_capable<Connection> {
      return xpp::xf86dri::query_direct_rendering_capable(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_direct_rendering_capable_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_direct_rendering_capable<Connection> {
      return xpp::xf86dri::query_direct_rendering_capable_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto open_connection(Parameter && ... parameter) const -> reply::checked::open_connection<Connection> {
      return xpp::xf86dri::open_connection(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto open_connection_unchecked(Parameter && ... parameter) const -> reply::unchecked::open_connection<Connection> {
      return xpp::xf86dri::open_connection_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void close_connection_checked(Parameter && ... parameter) const {
      xpp::xf86dri::close_connection_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void close_connection(Parameter && ... parameter) const {
      xpp::xf86dri::close_connection(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_client_driver_name(Parameter && ... parameter) const -> reply::checked::get_client_driver_name<Connection> {
      return xpp::xf86dri::get_client_driver_name(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_client_driver_name_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_client_driver_name<Connection> {
      return xpp::xf86dri::get_client_driver_name_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_context(Parameter && ... parameter) const -> reply::checked::create_context<Connection> {
      return xpp::xf86dri::create_context(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_context_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_context<Connection> {
      return xpp::xf86dri::create_context_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_context_checked(Parameter && ... parameter) const {
      xpp::xf86dri::destroy_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_context(Parameter && ... parameter) const {
      xpp::xf86dri::destroy_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_drawable(Parameter && ... parameter) const -> reply::checked::create_drawable<Connection> {
      return xpp::xf86dri::create_drawable(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_drawable_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_drawable<Connection> {
      return xpp::xf86dri::create_drawable_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_drawable_checked(Parameter && ... parameter) const {
      xpp::xf86dri::destroy_drawable_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_drawable(Parameter && ... parameter) const {
      xpp::xf86dri::destroy_drawable(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_drawable_info(Parameter && ... parameter) const -> reply::checked::get_drawable_info<Connection> {
      return xpp::xf86dri::get_drawable_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_drawable_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_drawable_info<Connection> {
      return xpp::xf86dri::get_drawable_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_device_info(Parameter && ... parameter) const -> reply::checked::get_device_info<Connection> {
      return xpp::xf86dri::get_device_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_device_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_device_info<Connection> {
      return xpp::xf86dri::get_device_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto auth_connection(Parameter && ... parameter) const -> reply::checked::auth_connection<Connection> {
      return xpp::xf86dri::auth_connection(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto auth_connection_unchecked(Parameter && ... parameter) const -> reply::unchecked::auth_connection<Connection> {
      return xpp::xf86dri::auth_connection_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::xf86dri::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::xf86dri::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::xf86dri::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::xf86dri::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_XF86DRI_HPP
