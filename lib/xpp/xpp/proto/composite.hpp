#ifndef XPP_COMPOSITE_HPP
#define XPP_COMPOSITE_HPP

#include <string>
#include <vector>

#include <xcb/composite.h>

#include "xpp/generic.hpp"

namespace xpp::composite {

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
  : public xpp::generic::extension<extension, &xcb_composite_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_composite_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::composite::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::composite::event::dispatcher<Connection>;
    using error_dispatcher = xpp::composite::error::dispatcher;
};


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_composite_query_version_reply), xcb_composite_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_composite_query_version_reply), xcb_composite_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_composite_query_version), xcb_composite_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_composite_query_version_unchecked), xcb_composite_query_version_unchecked>>;
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

template<typename Connection, typename ... Parameter>
void redirect_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_redirect_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void redirect_window(Parameter && ... parameter) {
  xcb_composite_redirect_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void redirect_subwindows_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_redirect_subwindows_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void redirect_subwindows(Parameter && ... parameter) {
  xcb_composite_redirect_subwindows(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void unredirect_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_unredirect_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void unredirect_window(Parameter && ... parameter) {
  xcb_composite_unredirect_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void unredirect_subwindows_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_unredirect_subwindows_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void unredirect_subwindows(Parameter && ... parameter) {
  xcb_composite_unredirect_subwindows(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_region_from_border_clip_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_create_region_from_border_clip_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_region_from_border_clip(Parameter && ... parameter) {
  xcb_composite_create_region_from_border_clip(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void name_window_pixmap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_name_window_pixmap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void name_window_pixmap(Parameter && ... parameter) {
  xcb_composite_name_window_pixmap(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_overlay_window : public xpp::generic::reply<get_overlay_window<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_composite_get_overlay_window_reply), xcb_composite_get_overlay_window_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_overlay_window<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_composite_get_overlay_window_reply), xcb_composite_get_overlay_window_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_overlay_window(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType overlay_win(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->overlay_win),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->overlay_win,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_overlay_window = detail::get_overlay_window<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_composite_get_overlay_window), xcb_composite_get_overlay_window>>;
}

namespace unchecked {
template<typename Connection>
using get_overlay_window = detail::get_overlay_window<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_composite_get_overlay_window_unchecked), xcb_composite_get_overlay_window_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_overlay_window<Connection> get_overlay_window(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_overlay_window<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_overlay_window<Connection> get_overlay_window_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_overlay_window<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void release_overlay_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::composite::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_composite_release_overlay_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void release_overlay_window(Parameter && ... parameter) {
  xcb_composite_release_overlay_window(std::forward<Parameter>(parameter) ...);
}



































template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::composite::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    composite() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::composite::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::composite::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void redirect_window_checked(Parameter && ... parameter) const {
      xpp::composite::redirect_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void redirect_window(Parameter && ... parameter) const {
      xpp::composite::redirect_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void redirect_subwindows_checked(Parameter && ... parameter) const {
      xpp::composite::redirect_subwindows_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void redirect_subwindows(Parameter && ... parameter) const {
      xpp::composite::redirect_subwindows(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unredirect_window_checked(Parameter && ... parameter) const {
      xpp::composite::unredirect_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unredirect_window(Parameter && ... parameter) const {
      xpp::composite::unredirect_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unredirect_subwindows_checked(Parameter && ... parameter) const {
      xpp::composite::unredirect_subwindows_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unredirect_subwindows(Parameter && ... parameter) const {
      xpp::composite::unredirect_subwindows(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_region_from_border_clip_checked(Parameter && ... parameter) const {
      xpp::composite::create_region_from_border_clip_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_region_from_border_clip(Parameter && ... parameter) const {
      xpp::composite::create_region_from_border_clip(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void name_window_pixmap_checked(Parameter && ... parameter) const {
      xpp::composite::name_window_pixmap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void name_window_pixmap(Parameter && ... parameter) const {
      xpp::composite::name_window_pixmap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_overlay_window(Parameter && ... parameter) const -> reply::checked::get_overlay_window<Connection> {
      return xpp::composite::get_overlay_window(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_overlay_window_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_overlay_window<Connection> {
      return xpp::composite::get_overlay_window_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void release_overlay_window_checked(Parameter && ... parameter) const {
      xpp::composite::release_overlay_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void release_overlay_window(Parameter && ... parameter) const {
      xpp::composite::release_overlay_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::composite::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::composite::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::composite::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::composite::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_COMPOSITE_HPP
