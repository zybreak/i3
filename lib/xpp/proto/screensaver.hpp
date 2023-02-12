#ifndef XPP_SCREENSAVER_HPP
#define XPP_SCREENSAVER_HPP

#include <string>
#include <vector>

#include <xcb/screensaver.h>

#include "xpp/generic.hpp"

namespace xpp::screensaver {

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
  : public xpp::generic::extension<extension, &xcb_screensaver_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_screensaver_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::screensaver::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::screensaver::event::dispatcher<Connection>;
    using error_dispatcher = xpp::screensaver::error::dispatcher;
};

namespace event {
template<typename Connection>
class notify : public xpp::generic::event<xcb_screensaver_notify_event_t> {
  public:
    typedef xpp::screensaver::extension extension;

    typedef xpp::generic::event<xcb_screensaver_notify_event_t> base;

    template<typename C>
    notify(C && c,
           uint8_t first_event,
           const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~notify() = default;

    static uint8_t opcode() {
      return XCB_SCREENSAVER_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::screensaver::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_SCREENSAVER_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->root,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType window(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->window,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}



namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_screensaver_query_version_reply), xcb_screensaver_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_screensaver_query_version_reply), xcb_screensaver_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_screensaver_query_version), xcb_screensaver_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_screensaver_query_version_unchecked), xcb_screensaver_query_version_unchecked>>;
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
class query_info : public xpp::generic::reply<query_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_screensaver_query_info_reply), xcb_screensaver_query_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_screensaver_query_info_reply), xcb_screensaver_query_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType saver_window(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->saver_window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->saver_window,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using query_info = detail::query_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_screensaver_query_info), xcb_screensaver_query_info>>;
}

namespace unchecked {
template<typename Connection>
using query_info = detail::query_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_screensaver_query_info_unchecked), xcb_screensaver_query_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_info<Connection> query_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_info<Connection> query_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void select_input_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::screensaver::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_screensaver_select_input_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void select_input(Parameter && ... parameter) {
  xcb_screensaver_select_input(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_attributes_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::screensaver::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_screensaver_set_attributes_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_attributes(Parameter && ... parameter) {
  xcb_screensaver_set_attributes(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void unset_attributes_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::screensaver::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_screensaver_unset_attributes_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void unset_attributes(Parameter && ... parameter) {
  xcb_screensaver_unset_attributes(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void suspend_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::screensaver::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_screensaver_suspend_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void suspend(Parameter && ... parameter) {
  xcb_screensaver_suspend(std::forward<Parameter>(parameter) ...);
}



































template<typename Derived, typename Connection>
class interface
{
  protected:
    virtual Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

  public:
    typedef xpp::screensaver::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    screensaver() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::screensaver::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::screensaver::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_info(Parameter && ... parameter) const -> reply::checked::query_info<Connection> {
      return xpp::screensaver::query_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_info<Connection> {
      return xpp::screensaver::query_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void select_input_checked(Parameter && ... parameter) const {
      xpp::screensaver::select_input_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void select_input(Parameter && ... parameter) const {
      xpp::screensaver::select_input(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_attributes_checked(Parameter && ... parameter) const {
      xpp::screensaver::set_attributes_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_attributes(Parameter && ... parameter) const {
      xpp::screensaver::set_attributes(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unset_attributes_checked(Parameter && ... parameter) const {
      xpp::screensaver::unset_attributes_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unset_attributes(Parameter && ... parameter) const {
      xpp::screensaver::unset_attributes(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void suspend_checked(Parameter && ... parameter) const {
      xpp::screensaver::suspend_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void suspend(Parameter && ... parameter) const {
      xpp::screensaver::suspend(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::screensaver::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::screensaver::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_SCREENSAVER_NOTIFY:
          handler(xpp::screensaver::event::notify<Connection>(m_c, m_first_event, event));
          return true;

      };

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
    typedef xpp::screensaver::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::screensaver::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_SCREENSAVER_HPP
