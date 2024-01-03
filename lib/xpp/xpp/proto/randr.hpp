#ifndef XPP_RANDR_HPP
#define XPP_RANDR_HPP

#include <string>
#include <vector>

#include <xcb/randr.h>

#include "xpp/generic.hpp"

namespace xpp::randr {

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
  : public xpp::generic::extension<extension, &xcb_randr_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_randr_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::randr::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::randr::event::dispatcher<Connection>;
    using error_dispatcher = xpp::randr::error::dispatcher;
};

namespace event {
template<typename Connection>
class screen_change_notify : public xpp::generic::event<xcb_randr_screen_change_notify_event_t> {
  public:
    typedef xpp::randr::extension extension;

    typedef xpp::generic::event<xcb_randr_screen_change_notify_event_t> base;

    template<typename C>
    screen_change_notify(C && c,
                         uint8_t first_event,
                         const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~screen_change_notify() = default;

    static uint8_t opcode() {
      return XCB_RANDR_SCREEN_CHANGE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::randr::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_RANDR_SCREEN_CHANGE_NOTIFY");
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
    ReturnType request_window(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->request_window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->request_window,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class notify : public xpp::generic::event<xcb_randr_notify_event_t> {
  public:
    typedef xpp::randr::extension extension;

    typedef xpp::generic::event<xcb_randr_notify_event_t> base;

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
      return XCB_RANDR_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::randr::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_RANDR_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }
  protected:
    Connection m_c;
    const uint8_t m_first_event;
};
}



namespace error {
class bad_output : public xpp::generic::error<bad_output, xcb_randr_bad_output_error_t> {
  public:
    using xpp::generic::error<bad_output, xcb_randr_bad_output_error_t>::error;

    virtual ~bad_output() = default;

    static uint8_t opcode() {
      return XCB_RANDR_BAD_OUTPUT;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::randr::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RANDR_BAD_OUTPUT");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class bad_crtc : public xpp::generic::error<bad_crtc, xcb_randr_bad_crtc_error_t> {
  public:
    using xpp::generic::error<bad_crtc, xcb_randr_bad_crtc_error_t>::error;

    virtual ~bad_crtc() = default;

    static uint8_t opcode() {
      return XCB_RANDR_BAD_CRTC;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::randr::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RANDR_BAD_CRTC");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class bad_mode : public xpp::generic::error<bad_mode, xcb_randr_bad_mode_error_t> {
  public:
    using xpp::generic::error<bad_mode, xcb_randr_bad_mode_error_t>::error;

    virtual ~bad_mode() = default;

    static uint8_t opcode() {
      return XCB_RANDR_BAD_MODE;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::randr::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RANDR_BAD_MODE");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class bad_provider : public xpp::generic::error<bad_provider, xcb_randr_bad_provider_error_t> {
  public:
    using xpp::generic::error<bad_provider, xcb_randr_bad_provider_error_t>::error;

    virtual ~bad_provider() = default;

    static uint8_t opcode() {
      return XCB_RANDR_BAD_PROVIDER;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::randr::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RANDR_BAD_PROVIDER");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_version : public xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_query_version_reply), xcb_randr_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_query_version_reply), xcb_randr_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_query_version), xcb_randr_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_query_version_unchecked), xcb_randr_query_version_unchecked>>;
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
class set_screen_config : public xpp::generic::reply<set_screen_config<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_set_screen_config_reply), xcb_randr_set_screen_config_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_screen_config<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_set_screen_config_reply), xcb_randr_set_screen_config_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_screen_config(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_set_screen_config_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_timestamp_t timestamp, xcb_timestamp_t config_timestamp, uint16_t sizeID, uint16_t rotation, uint16_t rate) {
      return base::cookie(c, window, timestamp, config_timestamp, sizeID, rotation, rate);
    }

    static xcb_randr_set_screen_config_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, uint16_t sizeID, uint16_t rotation, uint16_t rate, xcb_timestamp_t timestamp = XCB_TIME_CURRENT_TIME, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, window, timestamp, config_timestamp, sizeID, rotation, rate);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->root,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using set_screen_config = detail::set_screen_config<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_set_screen_config), xcb_randr_set_screen_config>>;
}

namespace unchecked {
template<typename Connection>
using set_screen_config = detail::set_screen_config<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_set_screen_config_unchecked), xcb_randr_set_screen_config_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_screen_config<Connection> set_screen_config(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_screen_config<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_screen_config<Connection> set_screen_config_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_screen_config<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void select_input_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_select_input_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void select_input(Parameter && ... parameter) {
  xcb_randr_select_input(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_info : public xpp::generic::reply<get_screen_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_screen_info_reply), xcb_randr_get_screen_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_screen_info_reply), xcb_randr_get_screen_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_screen_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_screen_size_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_info_sizes), xcb_randr_get_screen_info_sizes>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_info_sizes_length), xcb_randr_get_screen_info_sizes_length>>>
    sizes() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_screen_size_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_info_sizes), xcb_randr_get_screen_info_sizes>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_info_sizes_length), xcb_randr_get_screen_info_sizes_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_refresh_rates_t,
                                              xpp::generic::signature<decltype(xcb_randr_refresh_rates_next), xcb_randr_refresh_rates_next>,
                                              xpp::generic::signature<decltype(xcb_randr_refresh_rates_sizeof), xcb_randr_refresh_rates_sizeof>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_info_rates_iterator), xcb_randr_get_screen_info_rates_iterator>>>
    rates() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_refresh_rates_t,
                                                       xpp::generic::signature<decltype(xcb_randr_refresh_rates_next), xcb_randr_refresh_rates_next>,
                                                       xpp::generic::signature<decltype(xcb_randr_refresh_rates_sizeof), xcb_randr_refresh_rates_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_info_rates_iterator), xcb_randr_get_screen_info_rates_iterator>>
                               >(this->m_c, this->get());
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType root(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->root),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->root,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_screen_info = detail::get_screen_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_info), xcb_randr_get_screen_info>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_info = detail::get_screen_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_info_unchecked), xcb_randr_get_screen_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_info<Connection> get_screen_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_info<Connection> get_screen_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_size_range : public xpp::generic::reply<get_screen_size_range<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_screen_size_range_reply), xcb_randr_get_screen_size_range_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_size_range<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_screen_size_range_reply), xcb_randr_get_screen_size_range_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_size_range(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_screen_size_range = detail::get_screen_size_range<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_size_range), xcb_randr_get_screen_size_range>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_size_range = detail::get_screen_size_range<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_size_range_unchecked), xcb_randr_get_screen_size_range_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_size_range<Connection> get_screen_size_range(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_size_range<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_size_range<Connection> get_screen_size_range_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_size_range<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_screen_size_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_set_screen_size_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_screen_size(Parameter && ... parameter) {
  xcb_randr_set_screen_size(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_resources : public xpp::generic::reply<get_screen_resources<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_screen_resources_reply), xcb_randr_get_screen_resources_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_resources<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_screen_resources_reply), xcb_randr_get_screen_resources_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_resources(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_crtc_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_crtcs), xcb_randr_get_screen_resources_crtcs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_crtcs_length), xcb_randr_get_screen_resources_crtcs_length>>>
    crtcs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_crtc_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_crtcs), xcb_randr_get_screen_resources_crtcs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_crtcs_length), xcb_randr_get_screen_resources_crtcs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_output_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_outputs), xcb_randr_get_screen_resources_outputs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_outputs_length), xcb_randr_get_screen_resources_outputs_length>>>
    outputs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_output_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_outputs), xcb_randr_get_screen_resources_outputs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_outputs_length), xcb_randr_get_screen_resources_outputs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_mode_info_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_modes), xcb_randr_get_screen_resources_modes>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_modes_length), xcb_randr_get_screen_resources_modes_length>>>
    modes() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_mode_info_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_modes), xcb_randr_get_screen_resources_modes>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_modes_length), xcb_randr_get_screen_resources_modes_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_names), xcb_randr_get_screen_resources_names>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_names_length), xcb_randr_get_screen_resources_names_length>>>
    names() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_names), xcb_randr_get_screen_resources_names>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_names_length), xcb_randr_get_screen_resources_names_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_screen_resources = detail::get_screen_resources<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_resources), xcb_randr_get_screen_resources>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_resources = detail::get_screen_resources<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_resources_unchecked), xcb_randr_get_screen_resources_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_resources<Connection> get_screen_resources(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_resources<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_resources<Connection> get_screen_resources_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_resources<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_output_info : public xpp::generic::reply<get_output_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_output_info_reply), xcb_randr_get_output_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_output_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_output_info_reply), xcb_randr_get_output_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_output_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_get_output_info_cookie_t cookie(xcb_connection_t * const c, xcb_randr_output_t output, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, output, config_timestamp);
    }

    xpp::generic::list<Connection, xcb_randr_get_output_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_crtc_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_crtcs), xcb_randr_get_output_info_crtcs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_crtcs_length), xcb_randr_get_output_info_crtcs_length>>>
    crtcs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_output_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_crtc_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_crtcs), xcb_randr_get_output_info_crtcs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_crtcs_length), xcb_randr_get_output_info_crtcs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_output_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_mode_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_modes), xcb_randr_get_output_info_modes>,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_modes_length), xcb_randr_get_output_info_modes_length>>>
    modes() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_output_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_mode_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_modes), xcb_randr_get_output_info_modes>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_modes_length), xcb_randr_get_output_info_modes_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_output_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_output_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_clones), xcb_randr_get_output_info_clones>,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_clones_length), xcb_randr_get_output_info_clones_length>>>
    clones() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_output_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_output_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_clones), xcb_randr_get_output_info_clones>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_clones_length), xcb_randr_get_output_info_clones_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_output_info_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_name), xcb_randr_get_output_info_name>,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_info_name_length), xcb_randr_get_output_info_name_length>>>
    name() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_output_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_name), xcb_randr_get_output_info_name>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_info_name_length), xcb_randr_get_output_info_name_length>>
                               >(this->m_c, this->get());
    }

    template<typename ReturnType = xcb_randr_crtc_t, typename ... Parameter>
    ReturnType crtc(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->crtc),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->crtc,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_output_info = detail::get_output_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_output_info), xcb_randr_get_output_info>>;
}

namespace unchecked {
template<typename Connection>
using get_output_info = detail::get_output_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_output_info_unchecked), xcb_randr_get_output_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_output_info<Connection> get_output_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_output_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_output_info<Connection> get_output_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_output_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_output_properties : public xpp::generic::reply<list_output_properties<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_list_output_properties_reply), xcb_randr_list_output_properties_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_output_properties<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_list_output_properties_reply), xcb_randr_list_output_properties_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_output_properties(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Atoms = xcb_atom_t>
    xpp::generic::list<Connection, xcb_randr_list_output_properties_reply_t, xpp::generic::iterator<Connection,
                                              Atoms,
                                              xpp::generic::signature<decltype(xcb_randr_list_output_properties_atoms), xcb_randr_list_output_properties_atoms>,
                                              xpp::generic::signature<decltype(xcb_randr_list_output_properties_atoms_length), xcb_randr_list_output_properties_atoms_length>>>
    atoms() {
      return xpp::generic::list<Connection,
                                xcb_randr_list_output_properties_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Atoms,
                                                       xpp::generic::signature<decltype(xcb_randr_list_output_properties_atoms), xcb_randr_list_output_properties_atoms>,
                                                       xpp::generic::signature<decltype(xcb_randr_list_output_properties_atoms_length), xcb_randr_list_output_properties_atoms_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_output_properties = detail::list_output_properties<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_list_output_properties), xcb_randr_list_output_properties>>;
}

namespace unchecked {
template<typename Connection>
using list_output_properties = detail::list_output_properties<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_list_output_properties_unchecked), xcb_randr_list_output_properties_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_output_properties<Connection> list_output_properties(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_output_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_output_properties<Connection> list_output_properties_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_output_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_output_property : public xpp::generic::reply<query_output_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_query_output_property_reply), xcb_randr_query_output_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_output_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_query_output_property_reply), xcb_randr_query_output_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_output_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_query_output_property_reply_t, xpp::generic::iterator<Connection,
                                              int32_t,
                                              xpp::generic::signature<decltype(xcb_randr_query_output_property_valid_values), xcb_randr_query_output_property_valid_values>,
                                              xpp::generic::signature<decltype(xcb_randr_query_output_property_valid_values_length), xcb_randr_query_output_property_valid_values_length>>>
    valid_values() {
      return xpp::generic::list<Connection,
                                xcb_randr_query_output_property_reply_t,
                                xpp::generic::iterator<Connection,
                                                       int32_t,
                                                       xpp::generic::signature<decltype(xcb_randr_query_output_property_valid_values), xcb_randr_query_output_property_valid_values>,
                                                       xpp::generic::signature<decltype(xcb_randr_query_output_property_valid_values_length), xcb_randr_query_output_property_valid_values_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_output_property = detail::query_output_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_query_output_property), xcb_randr_query_output_property>>;
}

namespace unchecked {
template<typename Connection>
using query_output_property = detail::query_output_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_query_output_property_unchecked), xcb_randr_query_output_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_output_property<Connection> query_output_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_output_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_output_property<Connection> query_output_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_output_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void configure_output_property_checked(Connection && c, xcb_randr_output_t output, xcb_atom_t property, uint8_t pending, uint8_t range, uint32_t values_len, const int32_t * values) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_configure_output_property_checked(std::forward<Connection>(c), output, property, pending, range, values_len, values));
}

template<typename Connection>
void configure_output_property(Connection && c, xcb_randr_output_t output, xcb_atom_t property, uint8_t pending, uint8_t range, uint32_t values_len, const int32_t * values) {
  xcb_randr_configure_output_property(std::forward<Connection>(c), output, property, pending, range, values_len, values);
}

template<typename Connection, typename Values_Iterator>
void configure_output_property_checked(Connection && c, xcb_randr_output_t output, xcb_atom_t property, uint8_t pending, uint8_t range, Values_Iterator values_begin, Values_Iterator values_end) {
      typedef typename value_type<Values_Iterator, ! std::is_pointer<Values_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> values =
        { value_iterator<Values_Iterator>(values_begin), value_iterator<Values_Iterator>(values_end) };
      
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_configure_output_property_checked(std::forward<Connection>(c), output, property, pending, range, static_cast<uint32_t>(values.size()), const_cast<const vector_type *>(values.data())));
}

template<typename Connection, typename Values_Iterator>
void configure_output_property(Connection && c, xcb_randr_output_t output, xcb_atom_t property, uint8_t pending, uint8_t range, Values_Iterator values_begin, Values_Iterator values_end) {
      typedef typename value_type<Values_Iterator, ! std::is_pointer<Values_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> values =
        { value_iterator<Values_Iterator>(values_begin), value_iterator<Values_Iterator>(values_end) };
      
  xcb_randr_configure_output_property(std::forward<Connection>(c), output, property, pending, range, static_cast<uint32_t>(values.size()), const_cast<const vector_type *>(values.data()));
}

template<typename Connection, typename ... Parameter>
void change_output_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_change_output_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_output_property(Parameter && ... parameter) {
  xcb_randr_change_output_property(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void delete_output_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_delete_output_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void delete_output_property(Parameter && ... parameter) {
  xcb_randr_delete_output_property(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_output_property : public xpp::generic::reply<get_output_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_output_property_reply), xcb_randr_get_output_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_output_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_output_property_reply), xcb_randr_get_output_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_output_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_output_property_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_property_data), xcb_randr_get_output_property_data>,
                                              xpp::generic::signature<decltype(xcb_randr_get_output_property_data_length), xcb_randr_get_output_property_data_length>>>
    data() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_output_property_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_property_data), xcb_randr_get_output_property_data>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_output_property_data_length), xcb_randr_get_output_property_data_length>>
                               >(this->m_c, this->get());
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType type(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->type),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->type,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_output_property = detail::get_output_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_output_property), xcb_randr_get_output_property>>;
}

namespace unchecked {
template<typename Connection>
using get_output_property = detail::get_output_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_output_property_unchecked), xcb_randr_get_output_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_output_property<Connection> get_output_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_output_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_output_property<Connection> get_output_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_output_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_mode : public xpp::generic::reply<create_mode<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_create_mode_reply), xcb_randr_create_mode_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_mode<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_create_mode_reply), xcb_randr_create_mode_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_mode(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_create_mode_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_randr_mode_info_t mode_info, uint32_t name_len, const char * name) {
      return base::cookie(c, window, mode_info, name_len, name);
    }

    static xcb_randr_create_mode_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_randr_mode_info_t mode_info, const std::string & name) {
      return base::cookie(c, window, mode_info, static_cast<uint32_t>(name.length()), name.c_str());
    }

    template<typename ReturnType = xcb_randr_mode_t, typename ... Parameter>
    ReturnType mode(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->mode),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->mode,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using create_mode = detail::create_mode<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_create_mode), xcb_randr_create_mode>>;
}

namespace unchecked {
template<typename Connection>
using create_mode = detail::create_mode<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_create_mode_unchecked), xcb_randr_create_mode_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_mode<Connection> create_mode(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_mode<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_mode<Connection> create_mode_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_mode<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_mode_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_destroy_mode_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_mode(Parameter && ... parameter) {
  xcb_randr_destroy_mode(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void add_output_mode_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_add_output_mode_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void add_output_mode(Parameter && ... parameter) {
  xcb_randr_add_output_mode(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void delete_output_mode_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_delete_output_mode_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void delete_output_mode(Parameter && ... parameter) {
  xcb_randr_delete_output_mode(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_crtc_info : public xpp::generic::reply<get_crtc_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_crtc_info_reply), xcb_randr_get_crtc_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_crtc_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_crtc_info_reply), xcb_randr_get_crtc_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_crtc_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_get_crtc_info_cookie_t cookie(xcb_connection_t * const c, xcb_randr_crtc_t crtc, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, crtc, config_timestamp);
    }

    xpp::generic::list<Connection, xcb_randr_get_crtc_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_output_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_info_outputs), xcb_randr_get_crtc_info_outputs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_info_outputs_length), xcb_randr_get_crtc_info_outputs_length>>>
    outputs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_output_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_info_outputs), xcb_randr_get_crtc_info_outputs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_info_outputs_length), xcb_randr_get_crtc_info_outputs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_crtc_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_output_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_info_possible), xcb_randr_get_crtc_info_possible>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_info_possible_length), xcb_randr_get_crtc_info_possible_length>>>
    possible() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_output_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_info_possible), xcb_randr_get_crtc_info_possible>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_info_possible_length), xcb_randr_get_crtc_info_possible_length>>
                               >(this->m_c, this->get());
    }

    template<typename ReturnType = xcb_randr_mode_t, typename ... Parameter>
    ReturnType mode(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->mode),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->mode,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_crtc_info = detail::get_crtc_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_info), xcb_randr_get_crtc_info>>;
}

namespace unchecked {
template<typename Connection>
using get_crtc_info = detail::get_crtc_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_info_unchecked), xcb_randr_get_crtc_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_crtc_info<Connection> get_crtc_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_crtc_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_crtc_info<Connection> get_crtc_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_crtc_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_crtc_config : public xpp::generic::reply<set_crtc_config<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_set_crtc_config_reply), xcb_randr_set_crtc_config_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_crtc_config<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_set_crtc_config_reply), xcb_randr_set_crtc_config_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_crtc_config(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_set_crtc_config_cookie_t cookie(xcb_connection_t * const c, xcb_randr_crtc_t crtc, xcb_timestamp_t timestamp, xcb_timestamp_t config_timestamp, int16_t x, int16_t y, xcb_randr_mode_t mode, uint16_t rotation, uint32_t outputs_len, const xcb_randr_output_t * outputs) {
      return base::cookie(c, crtc, timestamp, config_timestamp, x, y, mode, rotation, outputs_len, outputs);
    }

    static xcb_randr_set_crtc_config_cookie_t cookie(xcb_connection_t * const c, xcb_randr_crtc_t crtc, int16_t x, int16_t y, xcb_randr_mode_t mode, uint16_t rotation, uint32_t outputs_len, const xcb_randr_output_t * outputs, xcb_timestamp_t timestamp = XCB_TIME_CURRENT_TIME, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, crtc, timestamp, config_timestamp, x, y, mode, rotation, outputs_len, outputs);
    }

    template<typename Outputs_Iterator>
    static xcb_randr_set_crtc_config_cookie_t cookie(xcb_connection_t * const c, xcb_randr_crtc_t crtc, int16_t x, int16_t y, xcb_randr_mode_t mode, uint16_t rotation, Outputs_Iterator outputs_begin, Outputs_Iterator outputs_end, xcb_timestamp_t timestamp = XCB_TIME_CURRENT_TIME, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
      typedef typename value_type<Outputs_Iterator, ! std::is_pointer<Outputs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> outputs =
        { value_iterator<Outputs_Iterator>(outputs_begin), value_iterator<Outputs_Iterator>(outputs_end) };
      
      return base::cookie(c, crtc, timestamp, config_timestamp, x, y, mode, rotation, static_cast<uint32_t>(outputs.size()), const_cast<const vector_type *>(outputs.data()));
    }
};

}

namespace checked {
template<typename Connection>
using set_crtc_config = detail::set_crtc_config<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_set_crtc_config), xcb_randr_set_crtc_config>>;
}

namespace unchecked {
template<typename Connection>
using set_crtc_config = detail::set_crtc_config<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_set_crtc_config_unchecked), xcb_randr_set_crtc_config_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_crtc_config<Connection> set_crtc_config(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_crtc_config<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_crtc_config<Connection> set_crtc_config_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_crtc_config<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_crtc_gamma_size : public xpp::generic::reply<get_crtc_gamma_size<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_size_reply), xcb_randr_get_crtc_gamma_size_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_crtc_gamma_size<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_size_reply), xcb_randr_get_crtc_gamma_size_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_crtc_gamma_size(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_crtc_gamma_size = detail::get_crtc_gamma_size<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_size), xcb_randr_get_crtc_gamma_size>>;
}

namespace unchecked {
template<typename Connection>
using get_crtc_gamma_size = detail::get_crtc_gamma_size<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_size_unchecked), xcb_randr_get_crtc_gamma_size_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_crtc_gamma_size<Connection> get_crtc_gamma_size(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_crtc_gamma_size<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_crtc_gamma_size<Connection> get_crtc_gamma_size_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_crtc_gamma_size<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_crtc_gamma : public xpp::generic::reply<get_crtc_gamma<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_reply), xcb_randr_get_crtc_gamma_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_crtc_gamma<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_reply), xcb_randr_get_crtc_gamma_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_crtc_gamma(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_crtc_gamma_reply_t, xpp::generic::iterator<Connection,
                                              uint16_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_red), xcb_randr_get_crtc_gamma_red>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_red_length), xcb_randr_get_crtc_gamma_red_length>>>
    red() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_gamma_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint16_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_red), xcb_randr_get_crtc_gamma_red>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_red_length), xcb_randr_get_crtc_gamma_red_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_crtc_gamma_reply_t, xpp::generic::iterator<Connection,
                                              uint16_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_green), xcb_randr_get_crtc_gamma_green>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_green_length), xcb_randr_get_crtc_gamma_green_length>>>
    green() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_gamma_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint16_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_green), xcb_randr_get_crtc_gamma_green>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_green_length), xcb_randr_get_crtc_gamma_green_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_crtc_gamma_reply_t, xpp::generic::iterator<Connection,
                                              uint16_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_blue), xcb_randr_get_crtc_gamma_blue>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_blue_length), xcb_randr_get_crtc_gamma_blue_length>>>
    blue() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_gamma_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint16_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_blue), xcb_randr_get_crtc_gamma_blue>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_blue_length), xcb_randr_get_crtc_gamma_blue_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_crtc_gamma = detail::get_crtc_gamma<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma), xcb_randr_get_crtc_gamma>>;
}

namespace unchecked {
template<typename Connection>
using get_crtc_gamma = detail::get_crtc_gamma<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_gamma_unchecked), xcb_randr_get_crtc_gamma_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_crtc_gamma<Connection> get_crtc_gamma(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_crtc_gamma<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_crtc_gamma<Connection> get_crtc_gamma_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_crtc_gamma<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_crtc_gamma_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_set_crtc_gamma_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_crtc_gamma(Parameter && ... parameter) {
  xcb_randr_set_crtc_gamma(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_resources_current : public xpp::generic::reply<get_screen_resources_current<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_reply), xcb_randr_get_screen_resources_current_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_resources_current<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_reply), xcb_randr_get_screen_resources_current_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_resources_current(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_current_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_crtc_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_crtcs), xcb_randr_get_screen_resources_current_crtcs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_crtcs_length), xcb_randr_get_screen_resources_current_crtcs_length>>>
    crtcs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_current_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_crtc_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_crtcs), xcb_randr_get_screen_resources_current_crtcs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_crtcs_length), xcb_randr_get_screen_resources_current_crtcs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_current_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_output_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_outputs), xcb_randr_get_screen_resources_current_outputs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_outputs_length), xcb_randr_get_screen_resources_current_outputs_length>>>
    outputs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_current_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_output_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_outputs), xcb_randr_get_screen_resources_current_outputs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_outputs_length), xcb_randr_get_screen_resources_current_outputs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_current_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_mode_info_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_modes), xcb_randr_get_screen_resources_current_modes>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_modes_length), xcb_randr_get_screen_resources_current_modes_length>>>
    modes() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_current_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_mode_info_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_modes), xcb_randr_get_screen_resources_current_modes>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_modes_length), xcb_randr_get_screen_resources_current_modes_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_screen_resources_current_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_names), xcb_randr_get_screen_resources_current_names>,
                                              xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_names_length), xcb_randr_get_screen_resources_current_names_length>>>
    names() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_screen_resources_current_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_names), xcb_randr_get_screen_resources_current_names>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_names_length), xcb_randr_get_screen_resources_current_names_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_screen_resources_current = detail::get_screen_resources_current<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current), xcb_randr_get_screen_resources_current>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_resources_current = detail::get_screen_resources_current<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_screen_resources_current_unchecked), xcb_randr_get_screen_resources_current_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_resources_current<Connection> get_screen_resources_current(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_resources_current<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_resources_current<Connection> get_screen_resources_current_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_resources_current<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_crtc_transform_checked(Connection && c, xcb_randr_crtc_t crtc, xcb_render_transform_t transform, uint16_t filter_len, const char * filter_name, uint32_t filter_params_len, const xcb_render_fixed_t * filter_params) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_set_crtc_transform_checked(std::forward<Connection>(c), crtc, transform, filter_len, filter_name, filter_params_len, filter_params));
}

template<typename Connection>
void set_crtc_transform(Connection && c, xcb_randr_crtc_t crtc, xcb_render_transform_t transform, uint16_t filter_len, const char * filter_name, uint32_t filter_params_len, const xcb_render_fixed_t * filter_params) {
  xcb_randr_set_crtc_transform(std::forward<Connection>(c), crtc, transform, filter_len, filter_name, filter_params_len, filter_params);
}

template<typename Connection, typename Filter_params_Iterator>
void set_crtc_transform_checked(Connection && c, xcb_randr_crtc_t crtc, xcb_render_transform_t transform, uint16_t filter_len, const char * filter_name, Filter_params_Iterator filter_params_begin, Filter_params_Iterator filter_params_end) {
      typedef typename value_type<Filter_params_Iterator, ! std::is_pointer<Filter_params_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> filter_params =
        { value_iterator<Filter_params_Iterator>(filter_params_begin), value_iterator<Filter_params_Iterator>(filter_params_end) };
      
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_set_crtc_transform_checked(std::forward<Connection>(c), crtc, transform, filter_len, filter_name, static_cast<uint32_t>(filter_params.size()), const_cast<const vector_type *>(filter_params.data())));
}

template<typename Connection, typename Filter_params_Iterator>
void set_crtc_transform(Connection && c, xcb_randr_crtc_t crtc, xcb_render_transform_t transform, uint16_t filter_len, const char * filter_name, Filter_params_Iterator filter_params_begin, Filter_params_Iterator filter_params_end) {
      typedef typename value_type<Filter_params_Iterator, ! std::is_pointer<Filter_params_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> filter_params =
        { value_iterator<Filter_params_Iterator>(filter_params_begin), value_iterator<Filter_params_Iterator>(filter_params_end) };
      
  xcb_randr_set_crtc_transform(std::forward<Connection>(c), crtc, transform, filter_len, filter_name, static_cast<uint32_t>(filter_params.size()), const_cast<const vector_type *>(filter_params.data()));
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_crtc_transform : public xpp::generic::reply<get_crtc_transform<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_reply), xcb_randr_get_crtc_transform_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_crtc_transform<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_reply), xcb_randr_get_crtc_transform_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_crtc_transform(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    std::string pending_filter_name() {
      return std::string(xcb_randr_get_crtc_transform_pending_filter_name(this->get().get()), xcb_randr_get_crtc_transform_pending_filter_name_length(this->get().get()));
    }



    xpp::generic::list<Connection, xcb_randr_get_crtc_transform_reply_t, xpp::generic::iterator<Connection,
                                              xcb_render_fixed_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_pending_params), xcb_randr_get_crtc_transform_pending_params>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_pending_params_length), xcb_randr_get_crtc_transform_pending_params_length>>>
    pending_params() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_transform_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_render_fixed_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_pending_params), xcb_randr_get_crtc_transform_pending_params>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_pending_params_length), xcb_randr_get_crtc_transform_pending_params_length>>
                               >(this->m_c, this->get());
    }


    std::string current_filter_name() {
      return std::string(xcb_randr_get_crtc_transform_current_filter_name(this->get().get()), xcb_randr_get_crtc_transform_current_filter_name_length(this->get().get()));
    }



    xpp::generic::list<Connection, xcb_randr_get_crtc_transform_reply_t, xpp::generic::iterator<Connection,
                                              xcb_render_fixed_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_current_params), xcb_randr_get_crtc_transform_current_params>,
                                              xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_current_params_length), xcb_randr_get_crtc_transform_current_params_length>>>
    current_params() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_crtc_transform_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_render_fixed_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_current_params), xcb_randr_get_crtc_transform_current_params>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_current_params_length), xcb_randr_get_crtc_transform_current_params_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_crtc_transform = detail::get_crtc_transform<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_transform), xcb_randr_get_crtc_transform>>;
}

namespace unchecked {
template<typename Connection>
using get_crtc_transform = detail::get_crtc_transform<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_crtc_transform_unchecked), xcb_randr_get_crtc_transform_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_crtc_transform<Connection> get_crtc_transform(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_crtc_transform<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_crtc_transform<Connection> get_crtc_transform_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_crtc_transform<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_panning : public xpp::generic::reply<get_panning<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_panning_reply), xcb_randr_get_panning_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_panning<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_panning_reply), xcb_randr_get_panning_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_panning(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_panning = detail::get_panning<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_panning), xcb_randr_get_panning>>;
}

namespace unchecked {
template<typename Connection>
using get_panning = detail::get_panning<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_panning_unchecked), xcb_randr_get_panning_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_panning<Connection> get_panning(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_panning<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_panning<Connection> get_panning_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_panning<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_panning : public xpp::generic::reply<set_panning<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_set_panning_reply), xcb_randr_set_panning_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_panning<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_set_panning_reply), xcb_randr_set_panning_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_panning(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_set_panning_cookie_t cookie(xcb_connection_t * const c, xcb_randr_crtc_t crtc, xcb_timestamp_t timestamp, uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint16_t track_left, uint16_t track_top, uint16_t track_width, uint16_t track_height, int16_t border_left, int16_t border_top, int16_t border_right, int16_t border_bottom) {
      return base::cookie(c, crtc, timestamp, left, top, width, height, track_left, track_top, track_width, track_height, border_left, border_top, border_right, border_bottom);
    }

    static xcb_randr_set_panning_cookie_t cookie(xcb_connection_t * const c, xcb_randr_crtc_t crtc, uint16_t left, uint16_t top, uint16_t width, uint16_t height, uint16_t track_left, uint16_t track_top, uint16_t track_width, uint16_t track_height, int16_t border_left, int16_t border_top, int16_t border_right, int16_t border_bottom, xcb_timestamp_t timestamp = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, crtc, timestamp, left, top, width, height, track_left, track_top, track_width, track_height, border_left, border_top, border_right, border_bottom);
    }
};

}

namespace checked {
template<typename Connection>
using set_panning = detail::set_panning<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_set_panning), xcb_randr_set_panning>>;
}

namespace unchecked {
template<typename Connection>
using set_panning = detail::set_panning<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_set_panning_unchecked), xcb_randr_set_panning_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_panning<Connection> set_panning(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_panning<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_panning<Connection> set_panning_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_panning<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_output_primary_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_set_output_primary_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_output_primary(Parameter && ... parameter) {
  xcb_randr_set_output_primary(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_output_primary : public xpp::generic::reply<get_output_primary<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_output_primary_reply), xcb_randr_get_output_primary_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_output_primary<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_output_primary_reply), xcb_randr_get_output_primary_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_output_primary(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_randr_output_t, typename ... Parameter>
    ReturnType output(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->output),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->output,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_output_primary = detail::get_output_primary<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_output_primary), xcb_randr_get_output_primary>>;
}

namespace unchecked {
template<typename Connection>
using get_output_primary = detail::get_output_primary<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_output_primary_unchecked), xcb_randr_get_output_primary_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_output_primary<Connection> get_output_primary(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_output_primary<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_output_primary<Connection> get_output_primary_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_output_primary<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_providers : public xpp::generic::reply<get_providers<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_providers_reply), xcb_randr_get_providers_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_providers<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_providers_reply), xcb_randr_get_providers_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_providers(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_providers_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_provider_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_providers_providers), xcb_randr_get_providers_providers>,
                                              xpp::generic::signature<decltype(xcb_randr_get_providers_providers_length), xcb_randr_get_providers_providers_length>>>
    providers() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_providers_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_provider_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_providers_providers), xcb_randr_get_providers_providers>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_providers_providers_length), xcb_randr_get_providers_providers_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_providers = detail::get_providers<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_providers), xcb_randr_get_providers>>;
}

namespace unchecked {
template<typename Connection>
using get_providers = detail::get_providers<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_providers_unchecked), xcb_randr_get_providers_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_providers<Connection> get_providers(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_providers<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_providers<Connection> get_providers_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_providers<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_provider_info : public xpp::generic::reply<get_provider_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_provider_info_reply), xcb_randr_get_provider_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_provider_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_provider_info_reply), xcb_randr_get_provider_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_provider_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_randr_get_provider_info_cookie_t cookie(xcb_connection_t * const c, xcb_randr_provider_t provider, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, provider, config_timestamp);
    }

    xpp::generic::list<Connection, xcb_randr_get_provider_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_crtc_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_crtcs), xcb_randr_get_provider_info_crtcs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_crtcs_length), xcb_randr_get_provider_info_crtcs_length>>>
    crtcs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_provider_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_crtc_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_crtcs), xcb_randr_get_provider_info_crtcs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_crtcs_length), xcb_randr_get_provider_info_crtcs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_provider_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_output_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_outputs), xcb_randr_get_provider_info_outputs>,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_outputs_length), xcb_randr_get_provider_info_outputs_length>>>
    outputs() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_provider_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_output_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_outputs), xcb_randr_get_provider_info_outputs>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_outputs_length), xcb_randr_get_provider_info_outputs_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_provider_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_provider_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_providers), xcb_randr_get_provider_info_associated_providers>,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_providers_length), xcb_randr_get_provider_info_associated_providers_length>>>
    associated_providers() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_provider_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_provider_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_providers), xcb_randr_get_provider_info_associated_providers>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_providers_length), xcb_randr_get_provider_info_associated_providers_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_randr_get_provider_info_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_capability), xcb_randr_get_provider_info_associated_capability>,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_capability_length), xcb_randr_get_provider_info_associated_capability_length>>>
    associated_capability() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_provider_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_capability), xcb_randr_get_provider_info_associated_capability>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_info_associated_capability_length), xcb_randr_get_provider_info_associated_capability_length>>
                               >(this->m_c, this->get());
    }


    std::string name() {
      return std::string(xcb_randr_get_provider_info_name(this->get().get()), xcb_randr_get_provider_info_name_length(this->get().get()));
    }

};

}

namespace checked {
template<typename Connection>
using get_provider_info = detail::get_provider_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_provider_info), xcb_randr_get_provider_info>>;
}

namespace unchecked {
template<typename Connection>
using get_provider_info = detail::get_provider_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_provider_info_unchecked), xcb_randr_get_provider_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_provider_info<Connection> get_provider_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_provider_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_provider_info<Connection> get_provider_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_provider_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_provider_offload_sink_checked(Connection && c, xcb_randr_provider_t provider, xcb_randr_provider_t sink_provider, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_set_provider_offload_sink_checked(std::forward<Connection>(c), provider, sink_provider, config_timestamp));
}

template<typename Connection>
void set_provider_offload_sink(Connection && c, xcb_randr_provider_t provider, xcb_randr_provider_t sink_provider, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
  xcb_randr_set_provider_offload_sink(std::forward<Connection>(c), provider, sink_provider, config_timestamp);
}

template<typename Connection>
void set_provider_output_source_checked(Connection && c, xcb_randr_provider_t provider, xcb_randr_provider_t source_provider, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_set_provider_output_source_checked(std::forward<Connection>(c), provider, source_provider, config_timestamp));
}

template<typename Connection>
void set_provider_output_source(Connection && c, xcb_randr_provider_t provider, xcb_randr_provider_t source_provider, xcb_timestamp_t config_timestamp = XCB_TIME_CURRENT_TIME) {
  xcb_randr_set_provider_output_source(std::forward<Connection>(c), provider, source_provider, config_timestamp);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_provider_properties : public xpp::generic::reply<list_provider_properties<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_list_provider_properties_reply), xcb_randr_list_provider_properties_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_provider_properties<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_list_provider_properties_reply), xcb_randr_list_provider_properties_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_provider_properties(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Atoms = xcb_atom_t>
    xpp::generic::list<Connection, xcb_randr_list_provider_properties_reply_t, xpp::generic::iterator<Connection,
                                              Atoms,
                                              xpp::generic::signature<decltype(xcb_randr_list_provider_properties_atoms), xcb_randr_list_provider_properties_atoms>,
                                              xpp::generic::signature<decltype(xcb_randr_list_provider_properties_atoms_length), xcb_randr_list_provider_properties_atoms_length>>>
    atoms() {
      return xpp::generic::list<Connection,
                                xcb_randr_list_provider_properties_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Atoms,
                                                       xpp::generic::signature<decltype(xcb_randr_list_provider_properties_atoms), xcb_randr_list_provider_properties_atoms>,
                                                       xpp::generic::signature<decltype(xcb_randr_list_provider_properties_atoms_length), xcb_randr_list_provider_properties_atoms_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_provider_properties = detail::list_provider_properties<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_list_provider_properties), xcb_randr_list_provider_properties>>;
}

namespace unchecked {
template<typename Connection>
using list_provider_properties = detail::list_provider_properties<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_list_provider_properties_unchecked), xcb_randr_list_provider_properties_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_provider_properties<Connection> list_provider_properties(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_provider_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_provider_properties<Connection> list_provider_properties_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_provider_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_provider_property : public xpp::generic::reply<query_provider_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_query_provider_property_reply), xcb_randr_query_provider_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_provider_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_query_provider_property_reply), xcb_randr_query_provider_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_provider_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_query_provider_property_reply_t, xpp::generic::iterator<Connection,
                                              int32_t,
                                              xpp::generic::signature<decltype(xcb_randr_query_provider_property_valid_values), xcb_randr_query_provider_property_valid_values>,
                                              xpp::generic::signature<decltype(xcb_randr_query_provider_property_valid_values_length), xcb_randr_query_provider_property_valid_values_length>>>
    valid_values() {
      return xpp::generic::list<Connection,
                                xcb_randr_query_provider_property_reply_t,
                                xpp::generic::iterator<Connection,
                                                       int32_t,
                                                       xpp::generic::signature<decltype(xcb_randr_query_provider_property_valid_values), xcb_randr_query_provider_property_valid_values>,
                                                       xpp::generic::signature<decltype(xcb_randr_query_provider_property_valid_values_length), xcb_randr_query_provider_property_valid_values_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_provider_property = detail::query_provider_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_query_provider_property), xcb_randr_query_provider_property>>;
}

namespace unchecked {
template<typename Connection>
using query_provider_property = detail::query_provider_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_query_provider_property_unchecked), xcb_randr_query_provider_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_provider_property<Connection> query_provider_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_provider_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_provider_property<Connection> query_provider_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_provider_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void configure_provider_property_checked(Connection && c, xcb_randr_provider_t provider, xcb_atom_t property, uint8_t pending, uint8_t range, uint32_t values_len, const int32_t * values) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_configure_provider_property_checked(std::forward<Connection>(c), provider, property, pending, range, values_len, values));
}

template<typename Connection>
void configure_provider_property(Connection && c, xcb_randr_provider_t provider, xcb_atom_t property, uint8_t pending, uint8_t range, uint32_t values_len, const int32_t * values) {
  xcb_randr_configure_provider_property(std::forward<Connection>(c), provider, property, pending, range, values_len, values);
}

template<typename Connection, typename Values_Iterator>
void configure_provider_property_checked(Connection && c, xcb_randr_provider_t provider, xcb_atom_t property, uint8_t pending, uint8_t range, Values_Iterator values_begin, Values_Iterator values_end) {
      typedef typename value_type<Values_Iterator, ! std::is_pointer<Values_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> values =
        { value_iterator<Values_Iterator>(values_begin), value_iterator<Values_Iterator>(values_end) };
      
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(std::forward<Connection>(c), xcb_randr_configure_provider_property_checked(std::forward<Connection>(c), provider, property, pending, range, static_cast<uint32_t>(values.size()), const_cast<const vector_type *>(values.data())));
}

template<typename Connection, typename Values_Iterator>
void configure_provider_property(Connection && c, xcb_randr_provider_t provider, xcb_atom_t property, uint8_t pending, uint8_t range, Values_Iterator values_begin, Values_Iterator values_end) {
      typedef typename value_type<Values_Iterator, ! std::is_pointer<Values_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> values =
        { value_iterator<Values_Iterator>(values_begin), value_iterator<Values_Iterator>(values_end) };
      
  xcb_randr_configure_provider_property(std::forward<Connection>(c), provider, property, pending, range, static_cast<uint32_t>(values.size()), const_cast<const vector_type *>(values.data()));
}

template<typename Connection, typename ... Parameter>
void change_provider_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_change_provider_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_provider_property(Parameter && ... parameter) {
  xcb_randr_change_provider_property(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void delete_provider_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_delete_provider_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void delete_provider_property(Parameter && ... parameter) {
  xcb_randr_delete_provider_property(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_provider_property : public xpp::generic::reply<get_provider_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_provider_property_reply), xcb_randr_get_provider_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_provider_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_provider_property_reply), xcb_randr_get_provider_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_provider_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Type>
    xpp::generic::list<Connection, xcb_randr_get_provider_property_reply_t, xpp::generic::iterator<Connection,
                                              Type,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_property_data), xcb_randr_get_provider_property_data>,
                                              xpp::generic::signature<decltype(xcb_randr_get_provider_property_data_length), xcb_randr_get_provider_property_data_length>>>
    data() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_provider_property_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Type,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_property_data), xcb_randr_get_provider_property_data>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_provider_property_data_length), xcb_randr_get_provider_property_data_length>>
                               >(this->m_c, this->get());
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType type(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->type),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->type,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_provider_property = detail::get_provider_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_provider_property), xcb_randr_get_provider_property>>;
}

namespace unchecked {
template<typename Connection>
using get_provider_property = detail::get_provider_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_provider_property_unchecked), xcb_randr_get_provider_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_provider_property<Connection> get_provider_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_provider_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_provider_property<Connection> get_provider_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_provider_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_monitors : public xpp::generic::reply<get_monitors<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_get_monitors_reply), xcb_randr_get_monitors_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_monitors<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_get_monitors_reply), xcb_randr_get_monitors_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_monitors(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_randr_get_monitors_reply_t, xpp::generic::iterator<Connection,
                                              xcb_randr_monitor_info_t,
                                              xpp::generic::signature<decltype(xcb_randr_monitor_info_next), xcb_randr_monitor_info_next>,
                                              xpp::generic::signature<decltype(xcb_randr_monitor_info_sizeof), xcb_randr_monitor_info_sizeof>,
                                              xpp::generic::signature<decltype(xcb_randr_get_monitors_monitors_iterator), xcb_randr_get_monitors_monitors_iterator>>>
    monitors() {
      return xpp::generic::list<Connection,
                                xcb_randr_get_monitors_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_randr_monitor_info_t,
                                                       xpp::generic::signature<decltype(xcb_randr_monitor_info_next), xcb_randr_monitor_info_next>,
                                                       xpp::generic::signature<decltype(xcb_randr_monitor_info_sizeof), xcb_randr_monitor_info_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_randr_get_monitors_monitors_iterator), xcb_randr_get_monitors_monitors_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_monitors = detail::get_monitors<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_get_monitors), xcb_randr_get_monitors>>;
}

namespace unchecked {
template<typename Connection>
using get_monitors = detail::get_monitors<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_get_monitors_unchecked), xcb_randr_get_monitors_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_monitors<Connection> get_monitors(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_monitors<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_monitors<Connection> get_monitors_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_monitors<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_monitor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_set_monitor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_monitor(Parameter && ... parameter) {
  xcb_randr_set_monitor(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void delete_monitor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_delete_monitor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void delete_monitor(Parameter && ... parameter) {
  xcb_randr_delete_monitor(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class create_lease : public xpp::generic::reply<create_lease<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_randr_create_lease_reply), xcb_randr_create_lease_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<create_lease<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_randr_create_lease_reply), xcb_randr_create_lease_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    create_lease(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using create_lease = detail::create_lease<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_randr_create_lease), xcb_randr_create_lease>>;
}

namespace unchecked {
template<typename Connection>
using create_lease = detail::create_lease<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_randr_create_lease_unchecked), xcb_randr_create_lease_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::create_lease<Connection> create_lease(Connection && c, Parameter && ... parameter) {
  return reply::checked::create_lease<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::create_lease<Connection> create_lease_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::create_lease<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void free_lease_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::randr::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_randr_free_lease_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_lease(Parameter && ... parameter) {
  xcb_randr_free_lease(std::forward<Parameter>(parameter) ...);
}










template<typename Derived, typename Connection>
class mode
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_randr_mode_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~mode() = default;

    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::randr::destroy_mode_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::randr::destroy_mode(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};





template<typename Derived, typename Connection>
class provider
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_randr_provider_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~provider() = default;

    template<typename ... Parameter>
    auto get_info(Parameter && ... parameter) const -> reply::checked::get_provider_info<Connection> {
      return xpp::randr::get_provider_info(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_provider_info<Connection> {
      return xpp::randr::get_provider_info_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_offload_sink_checked(Parameter && ... parameter) const {
      xpp::randr::set_provider_offload_sink_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_offload_sink(Parameter && ... parameter) const {
      xpp::randr::set_provider_offload_sink(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_output_source_checked(Parameter && ... parameter) const {
      xpp::randr::set_provider_output_source_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_output_source(Parameter && ... parameter) const {
      xpp::randr::set_provider_output_source(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_properties(Parameter && ... parameter) const -> reply::checked::list_provider_properties<Connection> {
      return xpp::randr::list_provider_properties(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_provider_properties<Connection> {
      return xpp::randr::list_provider_properties_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_property(Parameter && ... parameter) const -> reply::checked::query_provider_property<Connection> {
      return xpp::randr::query_provider_property(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_provider_property<Connection> {
      return xpp::randr::query_provider_property_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void configure_property_checked(Parameter && ... parameter) const {
      xpp::randr::configure_provider_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void configure_property(Parameter && ... parameter) const {
      xpp::randr::configure_provider_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_property_checked(Parameter && ... parameter) const {
      xpp::randr::change_provider_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_property(Parameter && ... parameter) const {
      xpp::randr::change_provider_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_property_checked(Parameter && ... parameter) const {
      xpp::randr::delete_provider_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_property(Parameter && ... parameter) const {
      xpp::randr::delete_provider_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_property(Parameter && ... parameter) const -> reply::checked::get_provider_property<Connection> {
      return xpp::randr::get_provider_property(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_provider_property<Connection> {
      return xpp::randr::get_provider_property_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};











template<typename Derived, typename Connection>
class output
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_randr_output_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~output() = default;

    template<typename ... Parameter>
    auto get_info(Parameter && ... parameter) const -> reply::checked::get_output_info<Connection> {
      return xpp::randr::get_output_info(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_output_info<Connection> {
      return xpp::randr::get_output_info_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_properties(Parameter && ... parameter) const -> reply::checked::list_output_properties<Connection> {
      return xpp::randr::list_output_properties(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_output_properties<Connection> {
      return xpp::randr::list_output_properties_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_property(Parameter && ... parameter) const -> reply::checked::query_output_property<Connection> {
      return xpp::randr::query_output_property(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_output_property<Connection> {
      return xpp::randr::query_output_property_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void configure_property_checked(Parameter && ... parameter) const {
      xpp::randr::configure_output_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void configure_property(Parameter && ... parameter) const {
      xpp::randr::configure_output_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_property_checked(Parameter && ... parameter) const {
      xpp::randr::change_output_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_property(Parameter && ... parameter) const {
      xpp::randr::change_output_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_property_checked(Parameter && ... parameter) const {
      xpp::randr::delete_output_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_property(Parameter && ... parameter) const {
      xpp::randr::delete_output_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_property(Parameter && ... parameter) const -> reply::checked::get_output_property<Connection> {
      return xpp::randr::get_output_property(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_output_property<Connection> {
      return xpp::randr::get_output_property_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_mode_checked(Parameter && ... parameter) const {
      xpp::randr::add_output_mode_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add_mode(Parameter && ... parameter) const {
      xpp::randr::add_output_mode(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_mode_checked(Parameter && ... parameter) const {
      xpp::randr::delete_output_mode_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_mode(Parameter && ... parameter) const {
      xpp::randr::delete_output_mode(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};


template<typename Derived, typename Connection>
class crtc
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_randr_crtc_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~crtc() = default;

    template<typename ... Parameter>
    auto get_info(Parameter && ... parameter) const -> reply::checked::get_crtc_info<Connection> {
      return xpp::randr::get_crtc_info(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_info<Connection> {
      return xpp::randr::get_crtc_info_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_config(Parameter && ... parameter) const -> reply::checked::set_crtc_config<Connection> {
      return xpp::randr::set_crtc_config(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_config_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_crtc_config<Connection> {
      return xpp::randr::set_crtc_config_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_gamma_size(Parameter && ... parameter) const -> reply::checked::get_crtc_gamma_size<Connection> {
      return xpp::randr::get_crtc_gamma_size(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_gamma_size_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_gamma_size<Connection> {
      return xpp::randr::get_crtc_gamma_size_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_gamma(Parameter && ... parameter) const -> reply::checked::get_crtc_gamma<Connection> {
      return xpp::randr::get_crtc_gamma(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_gamma_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_gamma<Connection> {
      return xpp::randr::get_crtc_gamma_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_gamma_checked(Parameter && ... parameter) const {
      xpp::randr::set_crtc_gamma_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_gamma(Parameter && ... parameter) const {
      xpp::randr::set_crtc_gamma(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_transform_checked(Parameter && ... parameter) const {
      xpp::randr::set_crtc_transform_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_transform(Parameter && ... parameter) const {
      xpp::randr::set_crtc_transform(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_transform(Parameter && ... parameter) const -> reply::checked::get_crtc_transform<Connection> {
      return xpp::randr::get_crtc_transform(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_transform_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_transform<Connection> {
      return xpp::randr::get_crtc_transform_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_panning(Parameter && ... parameter) const -> reply::checked::get_panning<Connection> {
      return xpp::randr::get_panning(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_panning_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_panning<Connection> {
      return xpp::randr::get_panning_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_panning(Parameter && ... parameter) const -> reply::checked::set_panning<Connection> {
      return xpp::randr::set_panning(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_panning_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_panning<Connection> {
      return xpp::randr::set_panning_unchecked(
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
    typedef xpp::randr::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    randr() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::randr::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::randr::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_screen_config(Parameter && ... parameter) const -> reply::checked::set_screen_config<Connection> {
      return xpp::randr::set_screen_config(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_screen_config_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_screen_config<Connection> {
      return xpp::randr::set_screen_config_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void select_input_checked(Parameter && ... parameter) const {
      xpp::randr::select_input_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void select_input(Parameter && ... parameter) const {
      xpp::randr::select_input(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_info(Parameter && ... parameter) const -> reply::checked::get_screen_info<Connection> {
      return xpp::randr::get_screen_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_info<Connection> {
      return xpp::randr::get_screen_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_size_range(Parameter && ... parameter) const -> reply::checked::get_screen_size_range<Connection> {
      return xpp::randr::get_screen_size_range(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_size_range_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_size_range<Connection> {
      return xpp::randr::get_screen_size_range_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_screen_size_checked(Parameter && ... parameter) const {
      xpp::randr::set_screen_size_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_screen_size(Parameter && ... parameter) const {
      xpp::randr::set_screen_size(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_resources(Parameter && ... parameter) const -> reply::checked::get_screen_resources<Connection> {
      return xpp::randr::get_screen_resources(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_resources_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_resources<Connection> {
      return xpp::randr::get_screen_resources_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_output_info(Parameter && ... parameter) const -> reply::checked::get_output_info<Connection> {
      return xpp::randr::get_output_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_output_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_output_info<Connection> {
      return xpp::randr::get_output_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_output_properties(Parameter && ... parameter) const -> reply::checked::list_output_properties<Connection> {
      return xpp::randr::list_output_properties(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_output_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_output_properties<Connection> {
      return xpp::randr::list_output_properties_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_output_property(Parameter && ... parameter) const -> reply::checked::query_output_property<Connection> {
      return xpp::randr::query_output_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_output_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_output_property<Connection> {
      return xpp::randr::query_output_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void configure_output_property_checked(Parameter && ... parameter) const {
      xpp::randr::configure_output_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void configure_output_property(Parameter && ... parameter) const {
      xpp::randr::configure_output_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_output_property_checked(Parameter && ... parameter) const {
      xpp::randr::change_output_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_output_property(Parameter && ... parameter) const {
      xpp::randr::change_output_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_output_property_checked(Parameter && ... parameter) const {
      xpp::randr::delete_output_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_output_property(Parameter && ... parameter) const {
      xpp::randr::delete_output_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_output_property(Parameter && ... parameter) const -> reply::checked::get_output_property<Connection> {
      return xpp::randr::get_output_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_output_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_output_property<Connection> {
      return xpp::randr::get_output_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_mode(Parameter && ... parameter) const -> reply::checked::create_mode<Connection> {
      return xpp::randr::create_mode(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_mode_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_mode<Connection> {
      return xpp::randr::create_mode_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_mode_checked(Parameter && ... parameter) const {
      xpp::randr::destroy_mode_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_mode(Parameter && ... parameter) const {
      xpp::randr::destroy_mode(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_output_mode_checked(Parameter && ... parameter) const {
      xpp::randr::add_output_mode_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add_output_mode(Parameter && ... parameter) const {
      xpp::randr::add_output_mode(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_output_mode_checked(Parameter && ... parameter) const {
      xpp::randr::delete_output_mode_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_output_mode(Parameter && ... parameter) const {
      xpp::randr::delete_output_mode(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_crtc_info(Parameter && ... parameter) const -> reply::checked::get_crtc_info<Connection> {
      return xpp::randr::get_crtc_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_crtc_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_info<Connection> {
      return xpp::randr::get_crtc_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_crtc_config(Parameter && ... parameter) const -> reply::checked::set_crtc_config<Connection> {
      return xpp::randr::set_crtc_config(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_crtc_config_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_crtc_config<Connection> {
      return xpp::randr::set_crtc_config_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_crtc_gamma_size(Parameter && ... parameter) const -> reply::checked::get_crtc_gamma_size<Connection> {
      return xpp::randr::get_crtc_gamma_size(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_crtc_gamma_size_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_gamma_size<Connection> {
      return xpp::randr::get_crtc_gamma_size_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_crtc_gamma(Parameter && ... parameter) const -> reply::checked::get_crtc_gamma<Connection> {
      return xpp::randr::get_crtc_gamma(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_crtc_gamma_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_gamma<Connection> {
      return xpp::randr::get_crtc_gamma_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_crtc_gamma_checked(Parameter && ... parameter) const {
      xpp::randr::set_crtc_gamma_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_crtc_gamma(Parameter && ... parameter) const {
      xpp::randr::set_crtc_gamma(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_resources_current(Parameter && ... parameter) const -> reply::checked::get_screen_resources_current<Connection> {
      return xpp::randr::get_screen_resources_current(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_resources_current_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_resources_current<Connection> {
      return xpp::randr::get_screen_resources_current_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_crtc_transform_checked(Parameter && ... parameter) const {
      xpp::randr::set_crtc_transform_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_crtc_transform(Parameter && ... parameter) const {
      xpp::randr::set_crtc_transform(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_crtc_transform(Parameter && ... parameter) const -> reply::checked::get_crtc_transform<Connection> {
      return xpp::randr::get_crtc_transform(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_crtc_transform_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_crtc_transform<Connection> {
      return xpp::randr::get_crtc_transform_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_panning(Parameter && ... parameter) const -> reply::checked::get_panning<Connection> {
      return xpp::randr::get_panning(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_panning_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_panning<Connection> {
      return xpp::randr::get_panning_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_panning(Parameter && ... parameter) const -> reply::checked::set_panning<Connection> {
      return xpp::randr::set_panning(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_panning_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_panning<Connection> {
      return xpp::randr::set_panning_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_output_primary_checked(Parameter && ... parameter) const {
      xpp::randr::set_output_primary_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_output_primary(Parameter && ... parameter) const {
      xpp::randr::set_output_primary(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_output_primary(Parameter && ... parameter) const -> reply::checked::get_output_primary<Connection> {
      return xpp::randr::get_output_primary(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_output_primary_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_output_primary<Connection> {
      return xpp::randr::get_output_primary_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_providers(Parameter && ... parameter) const -> reply::checked::get_providers<Connection> {
      return xpp::randr::get_providers(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_providers_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_providers<Connection> {
      return xpp::randr::get_providers_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_provider_info(Parameter && ... parameter) const -> reply::checked::get_provider_info<Connection> {
      return xpp::randr::get_provider_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_provider_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_provider_info<Connection> {
      return xpp::randr::get_provider_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_provider_offload_sink_checked(Parameter && ... parameter) const {
      xpp::randr::set_provider_offload_sink_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_provider_offload_sink(Parameter && ... parameter) const {
      xpp::randr::set_provider_offload_sink(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_provider_output_source_checked(Parameter && ... parameter) const {
      xpp::randr::set_provider_output_source_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_provider_output_source(Parameter && ... parameter) const {
      xpp::randr::set_provider_output_source(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_provider_properties(Parameter && ... parameter) const -> reply::checked::list_provider_properties<Connection> {
      return xpp::randr::list_provider_properties(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_provider_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_provider_properties<Connection> {
      return xpp::randr::list_provider_properties_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_provider_property(Parameter && ... parameter) const -> reply::checked::query_provider_property<Connection> {
      return xpp::randr::query_provider_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_provider_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_provider_property<Connection> {
      return xpp::randr::query_provider_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void configure_provider_property_checked(Parameter && ... parameter) const {
      xpp::randr::configure_provider_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void configure_provider_property(Parameter && ... parameter) const {
      xpp::randr::configure_provider_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_provider_property_checked(Parameter && ... parameter) const {
      xpp::randr::change_provider_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_provider_property(Parameter && ... parameter) const {
      xpp::randr::change_provider_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_provider_property_checked(Parameter && ... parameter) const {
      xpp::randr::delete_provider_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_provider_property(Parameter && ... parameter) const {
      xpp::randr::delete_provider_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_provider_property(Parameter && ... parameter) const -> reply::checked::get_provider_property<Connection> {
      return xpp::randr::get_provider_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_provider_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_provider_property<Connection> {
      return xpp::randr::get_provider_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_monitors(Parameter && ... parameter) const -> reply::checked::get_monitors<Connection> {
      return xpp::randr::get_monitors(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_monitors_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_monitors<Connection> {
      return xpp::randr::get_monitors_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_monitor_checked(Parameter && ... parameter) const {
      xpp::randr::set_monitor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_monitor(Parameter && ... parameter) const {
      xpp::randr::set_monitor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_monitor_checked(Parameter && ... parameter) const {
      xpp::randr::delete_monitor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_monitor(Parameter && ... parameter) const {
      xpp::randr::delete_monitor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto create_lease(Parameter && ... parameter) const -> reply::checked::create_lease<Connection> {
      return xpp::randr::create_lease(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto create_lease_unchecked(Parameter && ... parameter) const -> reply::unchecked::create_lease<Connection> {
      return xpp::randr::create_lease_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_lease_checked(Parameter && ... parameter) const {
      xpp::randr::free_lease_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_lease(Parameter && ... parameter) const {
      xpp::randr::free_lease(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::randr::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::randr::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_RANDR_SCREEN_CHANGE_NOTIFY:
          handler(xpp::randr::event::screen_change_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_RANDR_NOTIFY:
          handler(xpp::randr::event::notify<Connection>(m_c, m_first_event, event));
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
    typedef xpp::randr::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::randr::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_RANDR_BAD_OUTPUT: // 0
          throw xpp::randr::error::bad_output(error);

        case XCB_RANDR_BAD_CRTC: // 1
          throw xpp::randr::error::bad_crtc(error);

        case XCB_RANDR_BAD_MODE: // 2
          throw xpp::randr::error::bad_mode(error);

        case XCB_RANDR_BAD_PROVIDER: // 3
          throw xpp::randr::error::bad_provider(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_RANDR_HPP
