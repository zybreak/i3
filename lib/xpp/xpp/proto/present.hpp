#ifndef XPP_PRESENT_HPP
#define XPP_PRESENT_HPP

#include <string>
#include <vector>

#include <xcb/present.h>

#include "xpp/generic.hpp"

namespace xpp::present {

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
  : public xpp::generic::extension<extension, &xcb_present_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_present_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::present::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::present::event::dispatcher<Connection>;
    using error_dispatcher = xpp::present::error::dispatcher;
};

namespace event {
template<typename Connection>
class generic : public xpp::generic::event<xcb_present_generic_event_t> {
  public:
    typedef xpp::present::extension extension;

    typedef xpp::generic::event<xcb_present_generic_event_t> base;

    template<typename C>
    generic(C && c,
            uint8_t first_event,
            const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~generic() = default;

    static uint8_t opcode() {
      return XCB_PRESENT_GENERIC;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::present::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_PRESENT_GENERIC");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_present_event_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class configure_notify : public xpp::generic::event<xcb_present_configure_notify_event_t> {
  public:
    typedef xpp::present::extension extension;

    typedef xpp::generic::event<xcb_present_configure_notify_event_t> base;

    template<typename C>
    configure_notify(C && c,
                     uint8_t first_event,
                     const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~configure_notify() = default;

    static uint8_t opcode() {
      return XCB_PRESENT_CONFIGURE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::present::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_PRESENT_CONFIGURE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_present_event_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
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


namespace event {
template<typename Connection>
class complete_notify : public xpp::generic::event<xcb_present_complete_notify_event_t> {
  public:
    typedef xpp::present::extension extension;

    typedef xpp::generic::event<xcb_present_complete_notify_event_t> base;

    template<typename C>
    complete_notify(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~complete_notify() = default;

    static uint8_t opcode() {
      return XCB_PRESENT_COMPLETE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::present::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_PRESENT_COMPLETE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_present_event_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
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


namespace event {
template<typename Connection>
class idle_notify : public xpp::generic::event<xcb_present_idle_notify_event_t> {
  public:
    typedef xpp::present::extension extension;

    typedef xpp::generic::event<xcb_present_idle_notify_event_t> base;

    template<typename C>
    idle_notify(C && c,
                uint8_t first_event,
                const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~idle_notify() = default;

    static uint8_t opcode() {
      return XCB_PRESENT_IDLE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::present::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_PRESENT_IDLE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_present_event_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
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

    template<typename ReturnType = xcb_pixmap_t, typename ... Parameter>
    ReturnType pixmap(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->pixmap),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->pixmap,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_sync_fence_t, typename ... Parameter>
    ReturnType idle_fence(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->idle_fence),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->idle_fence,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}


namespace event {
template<typename Connection>
class redirect_notify : public xpp::generic::event<xcb_present_redirect_notify_event_t> {
  public:
    typedef xpp::present::extension extension;

    typedef xpp::generic::event<xcb_present_redirect_notify_event_t> base;

    template<typename C>
    redirect_notify(C && c,
                    uint8_t first_event,
                    const std::shared_ptr<xcb_generic_event_t> & event)
      : base(event)
      , m_c(std::forward<C>(c))
      , m_first_event(first_event)
    {}

    virtual ~redirect_notify() = default;

    static uint8_t opcode() {
      return XCB_PRESENT_REDIRECT_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::present::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_PRESENT_REDIRECT_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_present_event_t, typename ... Parameter>
    ReturnType event(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType event_window(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->event_window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->event_window,
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

    template<typename ReturnType = xcb_pixmap_t, typename ... Parameter>
    ReturnType pixmap(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->pixmap),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->pixmap,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_xfixes_region_t, typename ... Parameter>
    ReturnType valid_region(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->valid_region),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->valid_region,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_xfixes_region_t, typename ... Parameter>
    ReturnType update_region(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->update_region),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->update_region,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_randr_crtc_t, typename ... Parameter>
    ReturnType target_crtc(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->target_crtc),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->target_crtc,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_sync_fence_t, typename ... Parameter>
    ReturnType wait_fence(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->wait_fence),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->wait_fence,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_sync_fence_t, typename ... Parameter>
    ReturnType idle_fence(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->idle_fence),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->idle_fence,
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
                               xpp::generic::signature<decltype(xcb_present_query_version_reply), xcb_present_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_present_query_version_reply), xcb_present_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_present_query_version), xcb_present_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_present_query_version_unchecked), xcb_present_query_version_unchecked>>;
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

template<typename Connection>
void pixmap_checked(Connection && c, xcb_window_t window, xcb_pixmap_t pixmap, uint32_t serial, xcb_xfixes_region_t valid, xcb_xfixes_region_t update, int16_t x_off, int16_t y_off, xcb_randr_crtc_t target_crtc, xcb_sync_fence_t wait_fence, xcb_sync_fence_t idle_fence, uint32_t options, uint64_t target_msc, uint64_t divisor, uint64_t remainder, uint32_t notifies_len, const xcb_present_notify_t * notifies) {
  xpp::generic::check<Connection, xpp::present::error::dispatcher>(std::forward<Connection>(c), xcb_present_pixmap_checked(std::forward<Connection>(c), window, pixmap, serial, valid, update, x_off, y_off, target_crtc, wait_fence, idle_fence, options, target_msc, divisor, remainder, notifies_len, notifies));
}

template<typename Connection>
void pixmap(Connection && c, xcb_window_t window, xcb_pixmap_t pixmap, uint32_t serial, xcb_xfixes_region_t valid, xcb_xfixes_region_t update, int16_t x_off, int16_t y_off, xcb_randr_crtc_t target_crtc, xcb_sync_fence_t wait_fence, xcb_sync_fence_t idle_fence, uint32_t options, uint64_t target_msc, uint64_t divisor, uint64_t remainder, uint32_t notifies_len, const xcb_present_notify_t * notifies) {
  xcb_present_pixmap(std::forward<Connection>(c), window, pixmap, serial, valid, update, x_off, y_off, target_crtc, wait_fence, idle_fence, options, target_msc, divisor, remainder, notifies_len, notifies);
}

template<typename Connection, typename Notifies_Iterator>
void pixmap_checked(Connection && c, xcb_window_t window, xcb_pixmap_t pixmap, uint32_t serial, xcb_xfixes_region_t valid, xcb_xfixes_region_t update, int16_t x_off, int16_t y_off, xcb_randr_crtc_t target_crtc, xcb_sync_fence_t wait_fence, xcb_sync_fence_t idle_fence, uint32_t options, uint64_t target_msc, uint64_t divisor, uint64_t remainder, Notifies_Iterator notifies_begin, Notifies_Iterator notifies_end) {
      typedef typename value_type<Notifies_Iterator, ! std::is_pointer<Notifies_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> notifies =
        { value_iterator<Notifies_Iterator>(notifies_begin), value_iterator<Notifies_Iterator>(notifies_end) };
      
  xpp::generic::check<Connection, xpp::present::error::dispatcher>(std::forward<Connection>(c), xcb_present_pixmap_checked(std::forward<Connection>(c), window, pixmap, serial, valid, update, x_off, y_off, target_crtc, wait_fence, idle_fence, options, target_msc, divisor, remainder, static_cast<uint32_t>(notifies.size()), const_cast<const vector_type *>(notifies.data())));
}

template<typename Connection, typename Notifies_Iterator>
void pixmap(Connection && c, xcb_window_t window, xcb_pixmap_t pixmap, uint32_t serial, xcb_xfixes_region_t valid, xcb_xfixes_region_t update, int16_t x_off, int16_t y_off, xcb_randr_crtc_t target_crtc, xcb_sync_fence_t wait_fence, xcb_sync_fence_t idle_fence, uint32_t options, uint64_t target_msc, uint64_t divisor, uint64_t remainder, Notifies_Iterator notifies_begin, Notifies_Iterator notifies_end) {
      typedef typename value_type<Notifies_Iterator, ! std::is_pointer<Notifies_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> notifies =
        { value_iterator<Notifies_Iterator>(notifies_begin), value_iterator<Notifies_Iterator>(notifies_end) };
      
  xcb_present_pixmap(std::forward<Connection>(c), window, pixmap, serial, valid, update, x_off, y_off, target_crtc, wait_fence, idle_fence, options, target_msc, divisor, remainder, static_cast<uint32_t>(notifies.size()), const_cast<const vector_type *>(notifies.data()));
}

template<typename Connection, typename ... Parameter>
void notify_msc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::present::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_present_notify_msc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void notify_msc(Parameter && ... parameter) {
  xcb_present_notify_msc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void select_input_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::present::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_present_select_input_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void select_input(Parameter && ... parameter) {
  xcb_present_select_input(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_capabilities : public xpp::generic::reply<query_capabilities<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_present_query_capabilities_reply), xcb_present_query_capabilities_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_capabilities<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_present_query_capabilities_reply), xcb_present_query_capabilities_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_capabilities(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_capabilities = detail::query_capabilities<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_present_query_capabilities), xcb_present_query_capabilities>>;
}

namespace unchecked {
template<typename Connection>
using query_capabilities = detail::query_capabilities<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_present_query_capabilities_unchecked), xcb_present_query_capabilities_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_capabilities<Connection> query_capabilities(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_capabilities<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_capabilities<Connection> query_capabilities_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_capabilities<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}
























template<typename Derived, typename Connection>
class event
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_present_event_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~event() = default;

    template<typename ... Parameter>
    void select_input_checked(Parameter && ... parameter) const {
      xpp::present::select_input_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void select_input(Parameter && ... parameter) const {
      xpp::present::select_input(connection(),
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
    typedef xpp::present::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    present() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::present::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::present::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void pixmap_checked(Parameter && ... parameter) const {
      xpp::present::pixmap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void pixmap(Parameter && ... parameter) const {
      xpp::present::pixmap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void notify_msc_checked(Parameter && ... parameter) const {
      xpp::present::notify_msc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void notify_msc(Parameter && ... parameter) const {
      xpp::present::notify_msc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void select_input_checked(Parameter && ... parameter) const {
      xpp::present::select_input_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void select_input(Parameter && ... parameter) const {
      xpp::present::select_input(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_capabilities(Parameter && ... parameter) const -> reply::checked::query_capabilities<Connection> {
      return xpp::present::query_capabilities(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_capabilities_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_capabilities<Connection> {
      return xpp::present::query_capabilities_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::present::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::present::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_PRESENT_CONFIGURE_NOTIFY:
          handler(xpp::present::event::configure_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_PRESENT_COMPLETE_NOTIFY:
          handler(xpp::present::event::complete_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_PRESENT_IDLE_NOTIFY:
          handler(xpp::present::event::idle_notify<Connection>(m_c, m_first_event, event));
          return true;

        case XCB_PRESENT_REDIRECT_NOTIFY:
          handler(xpp::present::event::redirect_notify<Connection>(m_c, m_first_event, event));
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
    typedef xpp::present::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::present::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_PRESENT_HPP
