#ifndef XPP_XVMC_HPP
#define XPP_XVMC_HPP

#include <string>
#include <vector>

#include <xcb/xvmc.h>

#include "xpp/generic.hpp"

namespace xpp::xvmc {

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
  : public xpp::generic::extension<extension, &xcb_xvmc_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_xvmc_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::xvmc::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::xvmc::event::dispatcher<Connection>;
    using error_dispatcher = xpp::xvmc::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xvmc_query_version_reply), xcb_xvmc_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xvmc_query_version_reply), xcb_xvmc_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xvmc_query_version), xcb_xvmc_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xvmc_query_version_unchecked), xcb_xvmc_query_version_unchecked>>;
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
class list_surface_types : public xpp::generic::reply<list_surface_types<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_reply), xcb_xvmc_list_surface_types_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_surface_types<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_reply), xcb_xvmc_list_surface_types_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_surface_types(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xvmc_list_surface_types_reply_t, xpp::generic::iterator<Connection,
                                              xcb_xvmc_surface_info_t,
                                              xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_surfaces), xcb_xvmc_list_surface_types_surfaces>,
                                              xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_surfaces_length), xcb_xvmc_list_surface_types_surfaces_length>>>
    surfaces() {
      return xpp::generic::list<Connection,
                                xcb_xvmc_list_surface_types_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_xvmc_surface_info_t,
                                                       xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_surfaces), xcb_xvmc_list_surface_types_surfaces>,
                                                       xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_surfaces_length), xcb_xvmc_list_surface_types_surfaces_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_surface_types = detail::list_surface_types<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xvmc_list_surface_types), xcb_xvmc_list_surface_types>>;
}

namespace unchecked {
template<typename Connection>
using list_surface_types = detail::list_surface_types<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xvmc_list_surface_types_unchecked), xcb_xvmc_list_surface_types_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_surface_types<Connection> list_surface_types(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_surface_types<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_surface_types<Connection> list_surface_types_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_surface_types<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_context : public xpp::generic::reply<create_context<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xvmc_create_context_reply), xcb_xvmc_create_context_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_context<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xvmc_create_context_reply), xcb_xvmc_create_context_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_context(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xvmc_create_context_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_xvmc_create_context_priv_data), xcb_xvmc_create_context_priv_data>,
                                              xpp::generic::signature<decltype(xcb_xvmc_create_context_priv_data_length), xcb_xvmc_create_context_priv_data_length>>>
    priv_data() {
      return xpp::generic::list<Connection,
                                xcb_xvmc_create_context_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_xvmc_create_context_priv_data), xcb_xvmc_create_context_priv_data>,
                                                       xpp::generic::signature<decltype(xcb_xvmc_create_context_priv_data_length), xcb_xvmc_create_context_priv_data_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using create_context = detail::create_context<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xvmc_create_context), xcb_xvmc_create_context>>;
}

namespace unchecked {
template<typename Connection>
using create_context = detail::create_context<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xvmc_create_context_unchecked), xcb_xvmc_create_context_unchecked>>;
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
  xpp::generic::check<Connection, xpp::xvmc::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_xvmc_destroy_context_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_context(Parameter && ... parameter) {
  xcb_xvmc_destroy_context(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_surface : public xpp::generic::reply<create_surface<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xvmc_create_surface_reply), xcb_xvmc_create_surface_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_surface<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xvmc_create_surface_reply), xcb_xvmc_create_surface_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_surface(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xvmc_create_surface_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_xvmc_create_surface_priv_data), xcb_xvmc_create_surface_priv_data>,
                                              xpp::generic::signature<decltype(xcb_xvmc_create_surface_priv_data_length), xcb_xvmc_create_surface_priv_data_length>>>
    priv_data() {
      return xpp::generic::list<Connection,
                                xcb_xvmc_create_surface_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_xvmc_create_surface_priv_data), xcb_xvmc_create_surface_priv_data>,
                                                       xpp::generic::signature<decltype(xcb_xvmc_create_surface_priv_data_length), xcb_xvmc_create_surface_priv_data_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using create_surface = detail::create_surface<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xvmc_create_surface), xcb_xvmc_create_surface>>;
}

namespace unchecked {
template<typename Connection>
using create_surface = detail::create_surface<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xvmc_create_surface_unchecked), xcb_xvmc_create_surface_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_surface<Connection> create_surface(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_surface<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_surface<Connection> create_surface_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_surface<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_surface_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::xvmc::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_xvmc_destroy_surface_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_surface(Parameter && ... parameter) {
  xcb_xvmc_destroy_surface(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_subpicture : public xpp::generic::reply<create_subpicture<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_reply), xcb_xvmc_create_subpicture_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_subpicture<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_reply), xcb_xvmc_create_subpicture_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_subpicture(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xvmc_create_subpicture_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_priv_data), xcb_xvmc_create_subpicture_priv_data>,
                                              xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_priv_data_length), xcb_xvmc_create_subpicture_priv_data_length>>>
    priv_data() {
      return xpp::generic::list<Connection,
                                xcb_xvmc_create_subpicture_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_priv_data), xcb_xvmc_create_subpicture_priv_data>,
                                                       xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_priv_data_length), xcb_xvmc_create_subpicture_priv_data_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using create_subpicture = detail::create_subpicture<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xvmc_create_subpicture), xcb_xvmc_create_subpicture>>;
}

namespace unchecked {
template<typename Connection>
using create_subpicture = detail::create_subpicture<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xvmc_create_subpicture_unchecked), xcb_xvmc_create_subpicture_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_subpicture<Connection> create_subpicture(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_subpicture<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_subpicture<Connection> create_subpicture_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_subpicture<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_subpicture_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::xvmc::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_xvmc_destroy_subpicture_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_subpicture(Parameter && ... parameter) {
  xcb_xvmc_destroy_subpicture(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_subpicture_types : public xpp::generic::reply<list_subpicture_types<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_reply), xcb_xvmc_list_subpicture_types_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_subpicture_types<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_reply), xcb_xvmc_list_subpicture_types_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_subpicture_types(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_xvmc_list_subpicture_types_reply_t, xpp::generic::iterator<Connection,
                                              xcb_xv_image_format_info_t,
                                              xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_types), xcb_xvmc_list_subpicture_types_types>,
                                              xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_types_length), xcb_xvmc_list_subpicture_types_types_length>>>
    types() {
      return xpp::generic::list<Connection,
                                xcb_xvmc_list_subpicture_types_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_xv_image_format_info_t,
                                                       xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_types), xcb_xvmc_list_subpicture_types_types>,
                                                       xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_types_length), xcb_xvmc_list_subpicture_types_types_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_subpicture_types = detail::list_subpicture_types<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types), xcb_xvmc_list_subpicture_types>>;
}

namespace unchecked {
template<typename Connection>
using list_subpicture_types = detail::list_subpicture_types<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_xvmc_list_subpicture_types_unchecked), xcb_xvmc_list_subpicture_types_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_subpicture_types<Connection> list_subpicture_types(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_subpicture_types<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_subpicture_types<Connection> list_subpicture_types_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_subpicture_types<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}


template<typename Derived, typename Connection>
class context
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_xvmc_context_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~context() = default;

    template<typename ... Parameter>
    auto create(Parameter && ... parameter) const -> reply::checked::create_context<Connection> {
      return xpp::xvmc::create_context(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_context<Connection> {
      return xpp::xvmc::create_context_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::xvmc::destroy_context_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::xvmc::destroy_context(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};
















template<typename Derived, typename Connection>
class subpicture
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_xvmc_subpicture_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~subpicture() = default;

    template<typename ... Parameter>
    auto create(Parameter && ... parameter) const -> reply::checked::create_subpicture<Connection> {
      return xpp::xvmc::create_subpicture(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_subpicture<Connection> {
      return xpp::xvmc::create_subpicture_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::xvmc::destroy_subpicture_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::xvmc::destroy_subpicture(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};














template<typename Derived, typename Connection>
class surface
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_xvmc_surface_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~surface() = default;

    template<typename ... Parameter>
    auto create(Parameter && ... parameter) const -> reply::checked::create_surface<Connection> {
      return xpp::xvmc::create_surface(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_surface<Connection> {
      return xpp::xvmc::create_surface_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::xvmc::destroy_surface_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::xvmc::destroy_surface(connection(),
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
    typedef xpp::xvmc::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    xvmc() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::xvmc::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::xvmc::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_surface_types(Parameter && ... parameter) const -> reply::checked::list_surface_types<Connection> {
      return xpp::xvmc::list_surface_types(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_surface_types_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_surface_types<Connection> {
      return xpp::xvmc::list_surface_types_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_context(Parameter && ... parameter) const -> reply::checked::create_context<Connection> {
      return xpp::xvmc::create_context(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_context_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_context<Connection> {
      return xpp::xvmc::create_context_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_context_checked(Parameter && ... parameter) const {
      xpp::xvmc::destroy_context_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_context(Parameter && ... parameter) const {
      xpp::xvmc::destroy_context(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_surface(Parameter && ... parameter) const -> reply::checked::create_surface<Connection> {
      return xpp::xvmc::create_surface(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_surface_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_surface<Connection> {
      return xpp::xvmc::create_surface_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_surface_checked(Parameter && ... parameter) const {
      xpp::xvmc::destroy_surface_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_surface(Parameter && ... parameter) const {
      xpp::xvmc::destroy_surface(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_subpicture(Parameter && ... parameter) const -> reply::checked::create_subpicture<Connection> {
      return xpp::xvmc::create_subpicture(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_subpicture_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_subpicture<Connection> {
      return xpp::xvmc::create_subpicture_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_subpicture_checked(Parameter && ... parameter) const {
      xpp::xvmc::destroy_subpicture_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_subpicture(Parameter && ... parameter) const {
      xpp::xvmc::destroy_subpicture(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_subpicture_types(Parameter && ... parameter) const -> reply::checked::list_subpicture_types<Connection> {
      return xpp::xvmc::list_subpicture_types(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_subpicture_types_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_subpicture_types<Connection> {
      return xpp::xvmc::list_subpicture_types_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::xvmc::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::xvmc::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::xvmc::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::xvmc::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_XVMC_HPP
