#ifndef XPP_X_HPP
#define XPP_X_HPP

#include <string>
#include <vector>

#include <xcb/xcb.h>

#include "xpp/generic.hpp"

namespace xpp::x {

template<typename Derived, typename Connection>
class interface;

namespace event {
template<typename Connection>
class dispatcher;
}

namespace error {
class dispatcher;
}

class extension  {
  public:
    template<typename Derived, typename Connection>
    using interface = xpp::x::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::x::event::dispatcher<Connection>;
    using error_dispatcher = xpp::x::error::dispatcher;
};

namespace event {
template<typename Connection>
class key_press : public xpp::generic::event<xcb_key_press_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_key_press_event_t> base;

    template<typename C>
    key_press(C && c,
              const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~key_press() = default;

    static uint8_t opcode() {
      return XCB_KEY_PRESS;
    }

    static std::string description() {
      return std::string("XCB_KEY_PRESS");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class key_release : public xpp::generic::event<xcb_key_press_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_key_press_event_t> base;

    template<typename C>
    key_release(C && c,
                const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~key_release() = default;

    static uint8_t opcode() {
      return XCB_KEY_RELEASE;
    }

    static std::string description() {
      return std::string("XCB_KEY_RELEASE");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class button_press : public xpp::generic::event<xcb_button_press_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_button_press_event_t> base;

    template<typename C>
    button_press(C && c,
                 const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~button_press() = default;

    static uint8_t opcode() {
      return XCB_BUTTON_PRESS;
    }

    static std::string description() {
      return std::string("XCB_BUTTON_PRESS");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class button_release : public xpp::generic::event<xcb_button_press_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_button_press_event_t> base;

    template<typename C>
    button_release(C && c,
                   const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~button_release() = default;

    static uint8_t opcode() {
      return XCB_BUTTON_RELEASE;
    }

    static std::string description() {
      return std::string("XCB_BUTTON_RELEASE");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class motion_notify : public xpp::generic::event<xcb_motion_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_motion_notify_event_t> base;

    template<typename C>
    motion_notify(C && c,
                  const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~motion_notify() = default;

    static uint8_t opcode() {
      return XCB_MOTION_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_MOTION_NOTIFY");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class enter_notify : public xpp::generic::event<xcb_enter_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_enter_notify_event_t> base;

    template<typename C>
    enter_notify(C && c,
                 const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~enter_notify() = default;

    static uint8_t opcode() {
      return XCB_ENTER_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_ENTER_NOTIFY");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class leave_notify : public xpp::generic::event<xcb_enter_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_enter_notify_event_t> base;

    template<typename C>
    leave_notify(C && c,
                 const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~leave_notify() = default;

    static uint8_t opcode() {
      return XCB_LEAVE_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_LEAVE_NOTIFY");
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
    ReturnType child(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->child,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class focus_in : public xpp::generic::event<xcb_focus_in_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_focus_in_event_t> base;

    template<typename C>
    focus_in(C && c,
             const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~focus_in() = default;

    static uint8_t opcode() {
      return XCB_FOCUS_IN;
    }

    static std::string description() {
      return std::string("XCB_FOCUS_IN");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class focus_out : public xpp::generic::event<xcb_focus_in_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_focus_in_event_t> base;

    template<typename C>
    focus_out(C && c,
              const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~focus_out() = default;

    static uint8_t opcode() {
      return XCB_FOCUS_OUT;
    }

    static std::string description() {
      return std::string("XCB_FOCUS_OUT");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class keymap_notify : public xpp::generic::event<xcb_keymap_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_keymap_notify_event_t> base;

    template<typename C>
    keymap_notify(C && c,
                  const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~keymap_notify() = default;

    static uint8_t opcode() {
      return XCB_KEYMAP_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_KEYMAP_NOTIFY");
    }
  protected:
    Connection m_c;
};
}


namespace event {
template<typename Connection>
class expose : public xpp::generic::event<xcb_expose_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_expose_event_t> base;

    template<typename C>
    expose(C && c,
           const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~expose() = default;

    static uint8_t opcode() {
      return XCB_EXPOSE;
    }

    static std::string description() {
      return std::string("XCB_EXPOSE");
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
};



}


namespace event {
template<typename Connection>
class graphics_exposure : public xpp::generic::event<xcb_graphics_exposure_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_graphics_exposure_event_t> base;

    template<typename C>
    graphics_exposure(C && c,
                      const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~graphics_exposure() = default;

    static uint8_t opcode() {
      return XCB_GRAPHICS_EXPOSURE;
    }

    static std::string description() {
      return std::string("XCB_GRAPHICS_EXPOSURE");
    }

    template<typename ReturnType = xcb_drawable_t, typename ... Parameter>
    ReturnType drawable(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->drawable),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->drawable,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class no_exposure : public xpp::generic::event<xcb_no_exposure_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_no_exposure_event_t> base;

    template<typename C>
    no_exposure(C && c,
                const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~no_exposure() = default;

    static uint8_t opcode() {
      return XCB_NO_EXPOSURE;
    }

    static std::string description() {
      return std::string("XCB_NO_EXPOSURE");
    }

    template<typename ReturnType = xcb_drawable_t, typename ... Parameter>
    ReturnType drawable(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->drawable),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->drawable,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class visibility_notify : public xpp::generic::event<xcb_visibility_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_visibility_notify_event_t> base;

    template<typename C>
    visibility_notify(C && c,
                      const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~visibility_notify() = default;

    static uint8_t opcode() {
      return XCB_VISIBILITY_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_VISIBILITY_NOTIFY");
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
};



}


namespace event {
template<typename Connection>
class create_notify : public xpp::generic::event<xcb_create_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_create_notify_event_t> base;

    template<typename C>
    create_notify(C && c,
                  const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~create_notify() = default;

    static uint8_t opcode() {
      return XCB_CREATE_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_CREATE_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType parent(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->parent),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->parent,
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
};



}


namespace event {
template<typename Connection>
class destroy_notify : public xpp::generic::event<xcb_destroy_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_destroy_notify_event_t> base;

    template<typename C>
    destroy_notify(C && c,
                   const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~destroy_notify() = default;

    static uint8_t opcode() {
      return XCB_DESTROY_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_DESTROY_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class unmap_notify : public xpp::generic::event<xcb_unmap_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_unmap_notify_event_t> base;

    template<typename C>
    unmap_notify(C && c,
                 const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~unmap_notify() = default;

    static uint8_t opcode() {
      return XCB_UNMAP_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_UNMAP_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class map_notify : public xpp::generic::event<xcb_map_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_map_notify_event_t> base;

    template<typename C>
    map_notify(C && c,
               const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~map_notify() = default;

    static uint8_t opcode() {
      return XCB_MAP_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_MAP_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class map_request : public xpp::generic::event<xcb_map_request_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_map_request_event_t> base;

    template<typename C>
    map_request(C && c,
                const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~map_request() = default;

    static uint8_t opcode() {
      return XCB_MAP_REQUEST;
    }

    static std::string description() {
      return std::string("XCB_MAP_REQUEST");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType parent(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->parent),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->parent,
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
};



}


namespace event {
template<typename Connection>
class reparent_notify : public xpp::generic::event<xcb_reparent_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_reparent_notify_event_t> base;

    template<typename C>
    reparent_notify(C && c,
                    const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~reparent_notify() = default;

    static uint8_t opcode() {
      return XCB_REPARENT_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_REPARENT_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType parent(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->parent),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->parent,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class configure_notify : public xpp::generic::event<xcb_configure_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_configure_notify_event_t> base;

    template<typename C>
    configure_notify(C && c,
                     const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~configure_notify() = default;

    static uint8_t opcode() {
      return XCB_CONFIGURE_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_CONFIGURE_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType above_sibling(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->above_sibling),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->above_sibling,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class configure_request : public xpp::generic::event<xcb_configure_request_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_configure_request_event_t> base;

    template<typename C>
    configure_request(C && c,
                      const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~configure_request() = default;

    static uint8_t opcode() {
      return XCB_CONFIGURE_REQUEST;
    }

    static std::string description() {
      return std::string("XCB_CONFIGURE_REQUEST");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType parent(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->parent),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->parent,
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

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType sibling(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->sibling),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->sibling,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class gravity_notify : public xpp::generic::event<xcb_gravity_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_gravity_notify_event_t> base;

    template<typename C>
    gravity_notify(C && c,
                   const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~gravity_notify() = default;

    static uint8_t opcode() {
      return XCB_GRAVITY_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_GRAVITY_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class resize_request : public xpp::generic::event<xcb_resize_request_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_resize_request_event_t> base;

    template<typename C>
    resize_request(C && c,
                   const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~resize_request() = default;

    static uint8_t opcode() {
      return XCB_RESIZE_REQUEST;
    }

    static std::string description() {
      return std::string("XCB_RESIZE_REQUEST");
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
};



}


namespace event {
template<typename Connection>
class circulate_notify : public xpp::generic::event<xcb_circulate_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_circulate_notify_event_t> base;

    template<typename C>
    circulate_notify(C && c,
                     const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~circulate_notify() = default;

    static uint8_t opcode() {
      return XCB_CIRCULATE_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_CIRCULATE_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class circulate_request : public xpp::generic::event<xcb_circulate_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_circulate_notify_event_t> base;

    template<typename C>
    circulate_request(C && c,
                      const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~circulate_request() = default;

    static uint8_t opcode() {
      return XCB_CIRCULATE_REQUEST;
    }

    static std::string description() {
      return std::string("XCB_CIRCULATE_REQUEST");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
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
};



}


namespace event {
template<typename Connection>
class property_notify : public xpp::generic::event<xcb_property_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_property_notify_event_t> base;

    template<typename C>
    property_notify(C && c,
                    const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~property_notify() = default;

    static uint8_t opcode() {
      return XCB_PROPERTY_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_PROPERTY_NOTIFY");
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

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType atom(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->atom),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->atom,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class selection_clear : public xpp::generic::event<xcb_selection_clear_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_selection_clear_event_t> base;

    template<typename C>
    selection_clear(C && c,
                    const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~selection_clear() = default;

    static uint8_t opcode() {
      return XCB_SELECTION_CLEAR;
    }

    static std::string description() {
      return std::string("XCB_SELECTION_CLEAR");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType owner(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->owner),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->owner,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType selection(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->selection),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->selection,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class selection_request : public xpp::generic::event<xcb_selection_request_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_selection_request_event_t> base;

    template<typename C>
    selection_request(C && c,
                      const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~selection_request() = default;

    static uint8_t opcode() {
      return XCB_SELECTION_REQUEST;
    }

    static std::string description() {
      return std::string("XCB_SELECTION_REQUEST");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType owner(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->owner),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->owner,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType requestor(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->requestor),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->requestor,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType selection(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->selection),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->selection,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType target(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->target),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->target,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType property(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->property),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->property,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class selection_notify : public xpp::generic::event<xcb_selection_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_selection_notify_event_t> base;

    template<typename C>
    selection_notify(C && c,
                     const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~selection_notify() = default;

    static uint8_t opcode() {
      return XCB_SELECTION_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_SELECTION_NOTIFY");
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType requestor(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->requestor),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->requestor,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType selection(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->selection),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->selection,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType target(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->target),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->target,
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType property(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->property),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->property,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class colormap_notify : public xpp::generic::event<xcb_colormap_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_colormap_notify_event_t> base;

    template<typename C>
    colormap_notify(C && c,
                    const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~colormap_notify() = default;

    static uint8_t opcode() {
      return XCB_COLORMAP_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_COLORMAP_NOTIFY");
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

    template<typename ReturnType = xcb_colormap_t, typename ... Parameter>
    ReturnType colormap(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->colormap),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->colormap,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class client_message : public xpp::generic::event<xcb_client_message_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_client_message_event_t> base;

    template<typename C>
    client_message(C && c,
                   const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~client_message() = default;

    static uint8_t opcode() {
      return XCB_CLIENT_MESSAGE;
    }

    static std::string description() {
      return std::string("XCB_CLIENT_MESSAGE");
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

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType type(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->type),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->type,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
};



}


namespace event {
template<typename Connection>
class mapping_notify : public xpp::generic::event<xcb_mapping_notify_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_mapping_notify_event_t> base;

    template<typename C>
    mapping_notify(C && c,
                   const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~mapping_notify() = default;

    static uint8_t opcode() {
      return XCB_MAPPING_NOTIFY;
    }

    static std::string description() {
      return std::string("XCB_MAPPING_NOTIFY");
    }
  protected:
    Connection m_c;
};
}


namespace event {
template<typename Connection>
class ge_generic : public xpp::generic::event<xcb_ge_generic_event_t> {
  public:
    typedef xpp::x::extension extension;

    typedef xpp::generic::event<xcb_ge_generic_event_t> base;

    template<typename C>
    ge_generic(C && c,
               const std::shared_ptr<xcb_generic_event_t> & event) : base(event), m_c(std::forward<C>(c)) {}

    virtual ~ge_generic() = default;

    static uint8_t opcode() {
      return XCB_GE_GENERIC;
    }

    static std::string description() {
      return std::string("XCB_GE_GENERIC");
    }
  protected:
    Connection m_c;
};
}



namespace error {
class request : public xpp::generic::error<request, xcb_request_error_t> {
  public:
    using xpp::generic::error<request, xcb_request_error_t>::error;

    virtual ~request() = default;

    static uint8_t opcode() {
      return XCB_REQUEST;
    }

    static std::string description() {
      return std::string("XCB_REQUEST");
    }
};
} 


namespace error {
class value : public xpp::generic::error<value, xcb_value_error_t> {
  public:
    using xpp::generic::error<value, xcb_value_error_t>::error;

    virtual ~value() = default;

    static uint8_t opcode() {
      return XCB_VALUE;
    }

    static std::string description() {
      return std::string("XCB_VALUE");
    }
};
} 


namespace error {
class window : public xpp::generic::error<window, xcb_value_error_t> {
  public:
    using xpp::generic::error<window, xcb_value_error_t>::error;

    virtual ~window() = default;

    static uint8_t opcode() {
      return XCB_WINDOW;
    }

    static std::string description() {
      return std::string("XCB_WINDOW");
    }
};
} 


namespace error {
class pixmap : public xpp::generic::error<pixmap, xcb_value_error_t> {
  public:
    using xpp::generic::error<pixmap, xcb_value_error_t>::error;

    virtual ~pixmap() = default;

    static uint8_t opcode() {
      return XCB_PIXMAP;
    }

    static std::string description() {
      return std::string("XCB_PIXMAP");
    }
};
} 


namespace error {
class atom : public xpp::generic::error<atom, xcb_value_error_t> {
  public:
    using xpp::generic::error<atom, xcb_value_error_t>::error;

    virtual ~atom() = default;

    static uint8_t opcode() {
      return XCB_ATOM;
    }

    static std::string description() {
      return std::string("XCB_ATOM");
    }
};
} 


namespace error {
class cursor : public xpp::generic::error<cursor, xcb_value_error_t> {
  public:
    using xpp::generic::error<cursor, xcb_value_error_t>::error;

    virtual ~cursor() = default;

    static uint8_t opcode() {
      return XCB_CURSOR;
    }

    static std::string description() {
      return std::string("XCB_CURSOR");
    }
};
} 


namespace error {
class font : public xpp::generic::error<font, xcb_value_error_t> {
  public:
    using xpp::generic::error<font, xcb_value_error_t>::error;

    virtual ~font() = default;

    static uint8_t opcode() {
      return XCB_FONT;
    }

    static std::string description() {
      return std::string("XCB_FONT");
    }
};
} 


namespace error {
class match : public xpp::generic::error<match, xcb_request_error_t> {
  public:
    using xpp::generic::error<match, xcb_request_error_t>::error;

    virtual ~match() = default;

    static uint8_t opcode() {
      return XCB_MATCH;
    }

    static std::string description() {
      return std::string("XCB_MATCH");
    }
};
} 


namespace error {
class drawable : public xpp::generic::error<drawable, xcb_value_error_t> {
  public:
    using xpp::generic::error<drawable, xcb_value_error_t>::error;

    virtual ~drawable() = default;

    static uint8_t opcode() {
      return XCB_DRAWABLE;
    }

    static std::string description() {
      return std::string("XCB_DRAWABLE");
    }
};
} 


namespace error {
class access : public xpp::generic::error<access, xcb_request_error_t> {
  public:
    using xpp::generic::error<access, xcb_request_error_t>::error;

    virtual ~access() = default;

    static uint8_t opcode() {
      return XCB_ACCESS;
    }

    static std::string description() {
      return std::string("XCB_ACCESS");
    }
};
} 


namespace error {
class alloc : public xpp::generic::error<alloc, xcb_request_error_t> {
  public:
    using xpp::generic::error<alloc, xcb_request_error_t>::error;

    virtual ~alloc() = default;

    static uint8_t opcode() {
      return XCB_ALLOC;
    }

    static std::string description() {
      return std::string("XCB_ALLOC");
    }
};
} 


namespace error {
class colormap : public xpp::generic::error<colormap, xcb_value_error_t> {
  public:
    using xpp::generic::error<colormap, xcb_value_error_t>::error;

    virtual ~colormap() = default;

    static uint8_t opcode() {
      return XCB_COLORMAP;
    }

    static std::string description() {
      return std::string("XCB_COLORMAP");
    }
};
} 


namespace error {
class g_context : public xpp::generic::error<g_context, xcb_value_error_t> {
  public:
    using xpp::generic::error<g_context, xcb_value_error_t>::error;

    virtual ~g_context() = default;

    static uint8_t opcode() {
      return XCB_G_CONTEXT;
    }

    static std::string description() {
      return std::string("XCB_G_CONTEXT");
    }
};
} 


namespace error {
class id_choice : public xpp::generic::error<id_choice, xcb_value_error_t> {
  public:
    using xpp::generic::error<id_choice, xcb_value_error_t>::error;

    virtual ~id_choice() = default;

    static uint8_t opcode() {
      return XCB_ID_CHOICE;
    }

    static std::string description() {
      return std::string("XCB_ID_CHOICE");
    }
};
} 


namespace error {
class name : public xpp::generic::error<name, xcb_request_error_t> {
  public:
    using xpp::generic::error<name, xcb_request_error_t>::error;

    virtual ~name() = default;

    static uint8_t opcode() {
      return XCB_NAME;
    }

    static std::string description() {
      return std::string("XCB_NAME");
    }
};
} 


namespace error {
class length : public xpp::generic::error<length, xcb_request_error_t> {
  public:
    using xpp::generic::error<length, xcb_request_error_t>::error;

    virtual ~length() = default;

    static uint8_t opcode() {
      return XCB_LENGTH;
    }

    static std::string description() {
      return std::string("XCB_LENGTH");
    }
};
} 


namespace error {
class implementation : public xpp::generic::error<implementation, xcb_request_error_t> {
  public:
    using xpp::generic::error<implementation, xcb_request_error_t>::error;

    virtual ~implementation() = default;

    static uint8_t opcode() {
      return XCB_IMPLEMENTATION;
    }

    static std::string description() {
      return std::string("XCB_IMPLEMENTATION");
    }
};
} 


template<typename Connection, typename ... Parameter>
void create_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_create_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_window(Parameter && ... parameter) {
  xcb_create_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_window_attributes_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_change_window_attributes_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_window_attributes(Parameter && ... parameter) {
  xcb_change_window_attributes(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_window_attributes : public xpp::generic::reply<get_window_attributes<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_window_attributes_reply), xcb_get_window_attributes_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_window_attributes<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_window_attributes_reply), xcb_get_window_attributes_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_window_attributes(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_colormap_t, typename ... Parameter>
    ReturnType colormap(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->colormap),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->colormap,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_window_attributes = detail::get_window_attributes<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_window_attributes), xcb_get_window_attributes>>;
}

namespace unchecked {
template<typename Connection>
using get_window_attributes = detail::get_window_attributes<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_window_attributes_unchecked), xcb_get_window_attributes_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_window_attributes<Connection> get_window_attributes(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_window_attributes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_window_attributes<Connection> get_window_attributes_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_window_attributes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_destroy_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_window(Parameter && ... parameter) {
  xcb_destroy_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_subwindows_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_destroy_subwindows_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy_subwindows(Parameter && ... parameter) {
  xcb_destroy_subwindows(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_save_set_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_change_save_set_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_save_set(Parameter && ... parameter) {
  xcb_change_save_set(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void reparent_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_reparent_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void reparent_window(Parameter && ... parameter) {
  xcb_reparent_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void map_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_map_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void map_window(Parameter && ... parameter) {
  xcb_map_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void map_subwindows_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_map_subwindows_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void map_subwindows(Parameter && ... parameter) {
  xcb_map_subwindows(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void unmap_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_unmap_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void unmap_window(Parameter && ... parameter) {
  xcb_unmap_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void unmap_subwindows_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_unmap_subwindows_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void unmap_subwindows(Parameter && ... parameter) {
  xcb_unmap_subwindows(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void configure_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_configure_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void configure_window(Parameter && ... parameter) {
  xcb_configure_window(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void circulate_window_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_circulate_window_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void circulate_window(Parameter && ... parameter) {
  xcb_circulate_window(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_geometry : public xpp::generic::reply<get_geometry<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_geometry_reply), xcb_get_geometry_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_geometry<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_geometry_reply), xcb_get_geometry_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_geometry(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


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
using get_geometry = detail::get_geometry<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_geometry), xcb_get_geometry>>;
}

namespace unchecked {
template<typename Connection>
using get_geometry = detail::get_geometry<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_geometry_unchecked), xcb_get_geometry_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_geometry<Connection> get_geometry(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_geometry<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_geometry<Connection> get_geometry_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_geometry<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_tree : public xpp::generic::reply<query_tree<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_tree_reply), xcb_query_tree_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_tree<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_tree_reply), xcb_query_tree_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_tree(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Children = xcb_window_t>
    xpp::generic::list<Connection, xcb_query_tree_reply_t, xpp::generic::iterator<Connection,
                                              Children,
                                              xpp::generic::signature<decltype(xcb_query_tree_children), xcb_query_tree_children>,
                                              xpp::generic::signature<decltype(xcb_query_tree_children_length), xcb_query_tree_children_length>>>
    children() {
      return xpp::generic::list<Connection,
                                xcb_query_tree_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Children,
                                                       xpp::generic::signature<decltype(xcb_query_tree_children), xcb_query_tree_children>,
                                                       xpp::generic::signature<decltype(xcb_query_tree_children_length), xcb_query_tree_children_length>>
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

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType parent(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->parent),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->parent,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using query_tree = detail::query_tree<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_tree), xcb_query_tree>>;
}

namespace unchecked {
template<typename Connection>
using query_tree = detail::query_tree<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_tree_unchecked), xcb_query_tree_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_tree<Connection> query_tree(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_tree<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_tree<Connection> query_tree_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_tree<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class intern_atom : public xpp::generic::reply<intern_atom<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_intern_atom_reply), xcb_intern_atom_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<intern_atom<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_intern_atom_reply), xcb_intern_atom_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    intern_atom(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_intern_atom_cookie_t cookie(xcb_connection_t * const c, uint8_t only_if_exists, uint16_t name_len, const char * name) {
      return base::cookie(c, only_if_exists, name_len, name);
    }

    static xcb_intern_atom_cookie_t cookie(xcb_connection_t * const c, uint8_t only_if_exists, const std::string & name) {
      return base::cookie(c, only_if_exists, static_cast<uint16_t>(name.length()), name.c_str());
    }

    template<typename ReturnType = xcb_atom_t, typename ... Parameter>
    ReturnType atom(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->atom),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->atom,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using intern_atom = detail::intern_atom<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_intern_atom), xcb_intern_atom>>;
}

namespace unchecked {
template<typename Connection>
using intern_atom = detail::intern_atom<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_intern_atom_unchecked), xcb_intern_atom_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::intern_atom<Connection> intern_atom(Connection && c, Parameter && ... parameter) {
  return reply::checked::intern_atom<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::intern_atom<Connection> intern_atom_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::intern_atom<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_atom_name : public xpp::generic::reply<get_atom_name<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_atom_name_reply), xcb_get_atom_name_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_atom_name<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_atom_name_reply), xcb_get_atom_name_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_atom_name(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    std::string name() {
      return std::string(xcb_get_atom_name_name(this->get().get()), xcb_get_atom_name_name_length(this->get().get()));
    }

};

}

namespace checked {
template<typename Connection>
using get_atom_name = detail::get_atom_name<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_atom_name), xcb_get_atom_name>>;
}

namespace unchecked {
template<typename Connection>
using get_atom_name = detail::get_atom_name<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_atom_name_unchecked), xcb_get_atom_name_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_atom_name<Connection> get_atom_name(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_atom_name<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_atom_name<Connection> get_atom_name_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_atom_name<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void change_property_checked(Connection && c, uint8_t mode, xcb_window_t window, xcb_atom_t property, xcb_atom_t type, uint8_t format, uint32_t data_len, const void * data) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_change_property_checked(std::forward<Connection>(c), mode, window, property, type, format, data_len, data));
}

template<typename Connection>
void change_property(Connection && c, uint8_t mode, xcb_window_t window, xcb_atom_t property, xcb_atom_t type, uint8_t format, uint32_t data_len, const void * data) {
  xcb_change_property(std::forward<Connection>(c), mode, window, property, type, format, data_len, data);
}

template<typename Connection, typename Data_Iterator, typename Type_6 = typename Data_Iterator::value_type>
void change_property_checked(Connection && c, uint8_t mode, xcb_window_t window, xcb_atom_t property, xcb_atom_t type, uint8_t format, Data_Iterator data_begin, Data_Iterator data_end) {
      typedef typename value_type<Data_Iterator, ! std::is_pointer<Data_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> data =
        { value_iterator<Data_Iterator>(data_begin), value_iterator<Data_Iterator>(data_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_change_property_checked(std::forward<Connection>(c), mode, window, property, type, format, static_cast<uint32_t>(data.size()), const_cast<const vector_type *>(data.data())));
}

template<typename Connection, typename Data_Iterator, typename Type_6 = typename Data_Iterator::value_type>
void change_property(Connection && c, uint8_t mode, xcb_window_t window, xcb_atom_t property, xcb_atom_t type, uint8_t format, Data_Iterator data_begin, Data_Iterator data_end) {
      typedef typename value_type<Data_Iterator, ! std::is_pointer<Data_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> data =
        { value_iterator<Data_Iterator>(data_begin), value_iterator<Data_Iterator>(data_end) };
      
  xcb_change_property(std::forward<Connection>(c), mode, window, property, type, format, static_cast<uint32_t>(data.size()), const_cast<const vector_type *>(data.data()));
}

template<typename Connection, typename ... Parameter>
void delete_property_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_delete_property_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void delete_property(Parameter && ... parameter) {
  xcb_delete_property(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_property : public xpp::generic::reply<get_property<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_property_reply), xcb_get_property_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_property<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_property_reply), xcb_get_property_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_property(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Type>
    xpp::generic::list<Connection, xcb_get_property_reply_t, xpp::generic::iterator<Connection,
                                              Type,
                                              xpp::generic::signature<decltype(xcb_get_property_value), xcb_get_property_value>,
                                              xpp::generic::signature<decltype(xcb_get_property_value_length), xcb_get_property_value_length>>>
    value() {
      return xpp::generic::list<Connection,
                                xcb_get_property_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Type,
                                                       xpp::generic::signature<decltype(xcb_get_property_value), xcb_get_property_value>,
                                                       xpp::generic::signature<decltype(xcb_get_property_value_length), xcb_get_property_value_length>>
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
using get_property = detail::get_property<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_property), xcb_get_property>>;
}

namespace unchecked {
template<typename Connection>
using get_property = detail::get_property<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_property_unchecked), xcb_get_property_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_property<Connection> get_property(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_property<Connection> get_property_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_property<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_properties : public xpp::generic::reply<list_properties<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_list_properties_reply), xcb_list_properties_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_properties<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_list_properties_reply), xcb_list_properties_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_properties(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Atoms = xcb_atom_t>
    xpp::generic::list<Connection, xcb_list_properties_reply_t, xpp::generic::iterator<Connection,
                                              Atoms,
                                              xpp::generic::signature<decltype(xcb_list_properties_atoms), xcb_list_properties_atoms>,
                                              xpp::generic::signature<decltype(xcb_list_properties_atoms_length), xcb_list_properties_atoms_length>>>
    atoms() {
      return xpp::generic::list<Connection,
                                xcb_list_properties_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Atoms,
                                                       xpp::generic::signature<decltype(xcb_list_properties_atoms), xcb_list_properties_atoms>,
                                                       xpp::generic::signature<decltype(xcb_list_properties_atoms_length), xcb_list_properties_atoms_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_properties = detail::list_properties<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_list_properties), xcb_list_properties>>;
}

namespace unchecked {
template<typename Connection>
using list_properties = detail::list_properties<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_list_properties_unchecked), xcb_list_properties_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_properties<Connection> list_properties(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_properties<Connection> list_properties_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_properties<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_selection_owner_checked(Connection && c, xcb_window_t owner, xcb_atom_t selection, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_set_selection_owner_checked(std::forward<Connection>(c), owner, selection, time));
}

template<typename Connection>
void set_selection_owner(Connection && c, xcb_window_t owner, xcb_atom_t selection, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_set_selection_owner(std::forward<Connection>(c), owner, selection, time);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_selection_owner : public xpp::generic::reply<get_selection_owner<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_selection_owner_reply), xcb_get_selection_owner_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_selection_owner<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_selection_owner_reply), xcb_get_selection_owner_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_selection_owner(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType owner(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->owner),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->owner,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_selection_owner = detail::get_selection_owner<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_selection_owner), xcb_get_selection_owner>>;
}

namespace unchecked {
template<typename Connection>
using get_selection_owner = detail::get_selection_owner<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_selection_owner_unchecked), xcb_get_selection_owner_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_selection_owner<Connection> get_selection_owner(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_selection_owner<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_selection_owner<Connection> get_selection_owner_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_selection_owner<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void convert_selection_checked(Connection && c, xcb_window_t requestor, xcb_atom_t selection, xcb_atom_t target, xcb_atom_t property, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_convert_selection_checked(std::forward<Connection>(c), requestor, selection, target, property, time));
}

template<typename Connection>
void convert_selection(Connection && c, xcb_window_t requestor, xcb_atom_t selection, xcb_atom_t target, xcb_atom_t property, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_convert_selection(std::forward<Connection>(c), requestor, selection, target, property, time);
}

template<typename Connection, typename ... Parameter>
void send_event_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_send_event_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void send_event(Parameter && ... parameter) {
  xcb_send_event(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class grab_pointer : public xpp::generic::reply<grab_pointer<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_grab_pointer_reply), xcb_grab_pointer_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<grab_pointer<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_grab_pointer_reply), xcb_grab_pointer_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    grab_pointer(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_grab_pointer_cookie_t cookie(xcb_connection_t * const c, uint8_t owner_events, xcb_window_t grab_window, uint16_t event_mask, uint8_t pointer_mode, uint8_t keyboard_mode, xcb_window_t confine_to, xcb_cursor_t cursor, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, owner_events, grab_window, event_mask, pointer_mode, keyboard_mode, confine_to, cursor, time);
    }
};

}

namespace checked {
template<typename Connection>
using grab_pointer = detail::grab_pointer<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_grab_pointer), xcb_grab_pointer>>;
}

namespace unchecked {
template<typename Connection>
using grab_pointer = detail::grab_pointer<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_grab_pointer_unchecked), xcb_grab_pointer_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::grab_pointer<Connection> grab_pointer(Connection && c, Parameter && ... parameter) {
  return reply::checked::grab_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::grab_pointer<Connection> grab_pointer_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::grab_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void ungrab_pointer_checked(Connection && c, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_ungrab_pointer_checked(std::forward<Connection>(c), time));
}

template<typename Connection>
void ungrab_pointer(Connection && c, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_ungrab_pointer(std::forward<Connection>(c), time);
}

template<typename Connection, typename ... Parameter>
void grab_button_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_grab_button_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void grab_button(Parameter && ... parameter) {
  xcb_grab_button(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void ungrab_button_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_ungrab_button_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void ungrab_button(Parameter && ... parameter) {
  xcb_ungrab_button(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void change_active_pointer_grab_checked(Connection && c, xcb_cursor_t cursor, xcb_timestamp_t time, uint16_t event_mask) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_change_active_pointer_grab_checked(std::forward<Connection>(c), cursor, time, event_mask));
}

template<typename Connection>
void change_active_pointer_grab(Connection && c, xcb_cursor_t cursor, xcb_timestamp_t time, uint16_t event_mask) {
  xcb_change_active_pointer_grab(std::forward<Connection>(c), cursor, time, event_mask);
}

template<typename Connection>
void change_active_pointer_grab_checked(Connection && c, xcb_cursor_t cursor, uint16_t event_mask, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_change_active_pointer_grab_checked(std::forward<Connection>(c), cursor, time, event_mask));
}

template<typename Connection>
void change_active_pointer_grab(Connection && c, xcb_cursor_t cursor, uint16_t event_mask, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_change_active_pointer_grab(std::forward<Connection>(c), cursor, time, event_mask);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class grab_keyboard : public xpp::generic::reply<grab_keyboard<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_grab_keyboard_reply), xcb_grab_keyboard_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<grab_keyboard<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_grab_keyboard_reply), xcb_grab_keyboard_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    grab_keyboard(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_grab_keyboard_cookie_t cookie(xcb_connection_t * const c, uint8_t owner_events, xcb_window_t grab_window, xcb_timestamp_t time, uint8_t pointer_mode, uint8_t keyboard_mode) {
      return base::cookie(c, owner_events, grab_window, time, pointer_mode, keyboard_mode);
    }

    static xcb_grab_keyboard_cookie_t cookie(xcb_connection_t * const c, uint8_t owner_events, xcb_window_t grab_window, uint8_t pointer_mode, uint8_t keyboard_mode, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, owner_events, grab_window, time, pointer_mode, keyboard_mode);
    }
};

}

namespace checked {
template<typename Connection>
using grab_keyboard = detail::grab_keyboard<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_grab_keyboard), xcb_grab_keyboard>>;
}

namespace unchecked {
template<typename Connection>
using grab_keyboard = detail::grab_keyboard<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_grab_keyboard_unchecked), xcb_grab_keyboard_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::grab_keyboard<Connection> grab_keyboard(Connection && c, Parameter && ... parameter) {
  return reply::checked::grab_keyboard<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::grab_keyboard<Connection> grab_keyboard_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::grab_keyboard<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void ungrab_keyboard_checked(Connection && c, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_ungrab_keyboard_checked(std::forward<Connection>(c), time));
}

template<typename Connection>
void ungrab_keyboard(Connection && c, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_ungrab_keyboard(std::forward<Connection>(c), time);
}

template<typename Connection, typename ... Parameter>
void grab_key_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_grab_key_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void grab_key(Parameter && ... parameter) {
  xcb_grab_key(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void ungrab_key_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_ungrab_key_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void ungrab_key(Parameter && ... parameter) {
  xcb_ungrab_key(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void allow_events_checked(Connection && c, uint8_t mode, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_allow_events_checked(std::forward<Connection>(c), mode, time));
}

template<typename Connection>
void allow_events(Connection && c, uint8_t mode, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_allow_events(std::forward<Connection>(c), mode, time);
}

template<typename Connection, typename ... Parameter>
void grab_server_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_grab_server_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void grab_server(Parameter && ... parameter) {
  xcb_grab_server(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void ungrab_server_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_ungrab_server_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void ungrab_server(Parameter && ... parameter) {
  xcb_ungrab_server(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_pointer : public xpp::generic::reply<query_pointer<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_pointer_reply), xcb_query_pointer_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_pointer<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_pointer_reply), xcb_query_pointer_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_pointer(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


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

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->child,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using query_pointer = detail::query_pointer<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_pointer), xcb_query_pointer>>;
}

namespace unchecked {
template<typename Connection>
using query_pointer = detail::query_pointer<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_pointer_unchecked), xcb_query_pointer_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_pointer<Connection> query_pointer(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_pointer<Connection> query_pointer_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_pointer<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_motion_events : public xpp::generic::reply<get_motion_events<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_motion_events_reply), xcb_get_motion_events_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_motion_events<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_motion_events_reply), xcb_get_motion_events_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_motion_events(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_get_motion_events_cookie_t cookie(xcb_connection_t * const c, xcb_window_t window, xcb_timestamp_t start = XCB_TIME_CURRENT_TIME, xcb_timestamp_t stop = XCB_TIME_CURRENT_TIME) {
      return base::cookie(c, window, start, stop);
    }

    xpp::generic::list<Connection, xcb_get_motion_events_reply_t, xpp::generic::iterator<Connection,
                                              xcb_timecoord_t,
                                              xpp::generic::signature<decltype(xcb_get_motion_events_events), xcb_get_motion_events_events>,
                                              xpp::generic::signature<decltype(xcb_get_motion_events_events_length), xcb_get_motion_events_events_length>>>
    events() {
      return xpp::generic::list<Connection,
                                xcb_get_motion_events_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_timecoord_t,
                                                       xpp::generic::signature<decltype(xcb_get_motion_events_events), xcb_get_motion_events_events>,
                                                       xpp::generic::signature<decltype(xcb_get_motion_events_events_length), xcb_get_motion_events_events_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_motion_events = detail::get_motion_events<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_motion_events), xcb_get_motion_events>>;
}

namespace unchecked {
template<typename Connection>
using get_motion_events = detail::get_motion_events<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_motion_events_unchecked), xcb_get_motion_events_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_motion_events<Connection> get_motion_events(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_motion_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_motion_events<Connection> get_motion_events_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_motion_events<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class translate_coordinates : public xpp::generic::reply<translate_coordinates<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_translate_coordinates_reply), xcb_translate_coordinates_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<translate_coordinates<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_translate_coordinates_reply), xcb_translate_coordinates_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    translate_coordinates(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType child(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->child),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->child,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using translate_coordinates = detail::translate_coordinates<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_translate_coordinates), xcb_translate_coordinates>>;
}

namespace unchecked {
template<typename Connection>
using translate_coordinates = detail::translate_coordinates<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_translate_coordinates_unchecked), xcb_translate_coordinates_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::translate_coordinates<Connection> translate_coordinates(Connection && c, Parameter && ... parameter) {
  return reply::checked::translate_coordinates<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::translate_coordinates<Connection> translate_coordinates_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::translate_coordinates<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void warp_pointer_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_warp_pointer_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void warp_pointer(Parameter && ... parameter) {
  xcb_warp_pointer(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_input_focus_checked(Connection && c, uint8_t revert_to, xcb_window_t focus, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_set_input_focus_checked(std::forward<Connection>(c), revert_to, focus, time));
}

template<typename Connection>
void set_input_focus(Connection && c, uint8_t revert_to, xcb_window_t focus, xcb_timestamp_t time = XCB_TIME_CURRENT_TIME) {
  xcb_set_input_focus(std::forward<Connection>(c), revert_to, focus, time);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_input_focus : public xpp::generic::reply<get_input_focus<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_input_focus_reply), xcb_get_input_focus_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_input_focus<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_input_focus_reply), xcb_get_input_focus_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_input_focus(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType focus(Parameter && ... parameter) {
      using make = xpp::generic::factory::make<Connection,
                                               decltype(this->get()->focus),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    this->get()->focus,
                    std::forward<Parameter>(parameter) ...);
    }
};

}

namespace checked {
template<typename Connection>
using get_input_focus = detail::get_input_focus<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_input_focus), xcb_get_input_focus>>;
}

namespace unchecked {
template<typename Connection>
using get_input_focus = detail::get_input_focus<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_input_focus_unchecked), xcb_get_input_focus_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_input_focus<Connection> get_input_focus(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_input_focus<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_input_focus<Connection> get_input_focus_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_input_focus<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_keymap : public xpp::generic::reply<query_keymap<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_keymap_reply), xcb_query_keymap_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_keymap<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_keymap_reply), xcb_query_keymap_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_keymap(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_keymap = detail::query_keymap<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_keymap), xcb_query_keymap>>;
}

namespace unchecked {
template<typename Connection>
using query_keymap = detail::query_keymap<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_keymap_unchecked), xcb_query_keymap_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_keymap<Connection> query_keymap(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_keymap<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_keymap<Connection> query_keymap_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_keymap<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void open_font_checked(Connection && c, xcb_font_t fid, uint16_t name_len, const char * name) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_open_font_checked(std::forward<Connection>(c), fid, name_len, name));
}

template<typename Connection>
void open_font(Connection && c, xcb_font_t fid, uint16_t name_len, const char * name) {
  xcb_open_font(std::forward<Connection>(c), fid, name_len, name);
}

template<typename Connection>
void open_font_checked(Connection && c, xcb_font_t fid, const std::string & name) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_open_font_checked(std::forward<Connection>(c), fid, static_cast<uint16_t>(name.length()), name.c_str()));
}

template<typename Connection>
void open_font(Connection && c, xcb_font_t fid, const std::string & name) {
  xcb_open_font(std::forward<Connection>(c), fid, static_cast<uint16_t>(name.length()), name.c_str());
}

template<typename Connection, typename ... Parameter>
void close_font_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_close_font_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void close_font(Parameter && ... parameter) {
  xcb_close_font(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_font : public xpp::generic::reply<query_font<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_font_reply), xcb_query_font_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_font<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_font_reply), xcb_query_font_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_font(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_query_font_reply_t, xpp::generic::iterator<Connection,
                                              xcb_fontprop_t,
                                              xpp::generic::signature<decltype(xcb_query_font_properties), xcb_query_font_properties>,
                                              xpp::generic::signature<decltype(xcb_query_font_properties_length), xcb_query_font_properties_length>>>
    properties() {
      return xpp::generic::list<Connection,
                                xcb_query_font_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_fontprop_t,
                                                       xpp::generic::signature<decltype(xcb_query_font_properties), xcb_query_font_properties>,
                                                       xpp::generic::signature<decltype(xcb_query_font_properties_length), xcb_query_font_properties_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_query_font_reply_t, xpp::generic::iterator<Connection,
                                              xcb_charinfo_t,
                                              xpp::generic::signature<decltype(xcb_query_font_char_infos), xcb_query_font_char_infos>,
                                              xpp::generic::signature<decltype(xcb_query_font_char_infos_length), xcb_query_font_char_infos_length>>>
    char_infos() {
      return xpp::generic::list<Connection,
                                xcb_query_font_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_charinfo_t,
                                                       xpp::generic::signature<decltype(xcb_query_font_char_infos), xcb_query_font_char_infos>,
                                                       xpp::generic::signature<decltype(xcb_query_font_char_infos_length), xcb_query_font_char_infos_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_font = detail::query_font<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_font), xcb_query_font>>;
}

namespace unchecked {
template<typename Connection>
using query_font = detail::query_font<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_font_unchecked), xcb_query_font_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_font<Connection> query_font(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_font<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_font<Connection> query_font_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_font<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_text_extents : public xpp::generic::reply<query_text_extents<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_text_extents_reply), xcb_query_text_extents_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_text_extents<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_text_extents_reply), xcb_query_text_extents_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_text_extents(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_query_text_extents_cookie_t cookie(xcb_connection_t * const c, xcb_fontable_t font, uint32_t string_len, const xcb_char2b_t * string) {
      return base::cookie(c, font, string_len, string);
    }

    template<typename String_Iterator>
    static xcb_query_text_extents_cookie_t cookie(xcb_connection_t * const c, xcb_fontable_t font, String_Iterator string_begin, String_Iterator string_end) {
      typedef typename value_type<String_Iterator, ! std::is_pointer<String_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> string =
        { value_iterator<String_Iterator>(string_begin), value_iterator<String_Iterator>(string_end) };
      
      return base::cookie(c, font, static_cast<uint32_t>(string.size()), const_cast<const vector_type *>(string.data()));
    }
};

}

namespace checked {
template<typename Connection>
using query_text_extents = detail::query_text_extents<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_text_extents), xcb_query_text_extents>>;
}

namespace unchecked {
template<typename Connection>
using query_text_extents = detail::query_text_extents<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_text_extents_unchecked), xcb_query_text_extents_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_text_extents<Connection> query_text_extents(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_text_extents<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_text_extents<Connection> query_text_extents_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_text_extents<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_fonts : public xpp::generic::reply<list_fonts<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_list_fonts_reply), xcb_list_fonts_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_fonts<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_list_fonts_reply), xcb_list_fonts_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_fonts(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_list_fonts_cookie_t cookie(xcb_connection_t * const c, uint16_t max_names, uint16_t pattern_len, const char * pattern) {
      return base::cookie(c, max_names, pattern_len, pattern);
    }

    static xcb_list_fonts_cookie_t cookie(xcb_connection_t * const c, uint16_t max_names, const std::string & pattern) {
      return base::cookie(c, max_names, static_cast<uint16_t>(pattern.length()), pattern.c_str());
    }

    xpp::generic::list<Connection, xcb_list_fonts_reply_t, xpp::generic::iterator<Connection,
                                              xcb_str_t,
                                              xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                              xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                              xpp::generic::signature<decltype(xcb_list_fonts_names_iterator), xcb_list_fonts_names_iterator>>>
    names() {
      return xpp::generic::list<Connection,
                                xcb_list_fonts_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_str_t,
                                                       xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                                       xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_list_fonts_names_iterator), xcb_list_fonts_names_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_fonts = detail::list_fonts<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_list_fonts), xcb_list_fonts>>;
}

namespace unchecked {
template<typename Connection>
using list_fonts = detail::list_fonts<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_list_fonts_unchecked), xcb_list_fonts_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_fonts<Connection> list_fonts(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_fonts<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_fonts<Connection> list_fonts_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_fonts<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_fonts_with_info : public xpp::generic::reply<list_fonts_with_info<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_list_fonts_with_info_reply), xcb_list_fonts_with_info_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_fonts_with_info<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_list_fonts_with_info_reply), xcb_list_fonts_with_info_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_fonts_with_info(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_list_fonts_with_info_cookie_t cookie(xcb_connection_t * const c, uint16_t max_names, uint16_t pattern_len, const char * pattern) {
      return base::cookie(c, max_names, pattern_len, pattern);
    }

    static xcb_list_fonts_with_info_cookie_t cookie(xcb_connection_t * const c, uint16_t max_names, const std::string & pattern) {
      return base::cookie(c, max_names, static_cast<uint16_t>(pattern.length()), pattern.c_str());
    }

    xpp::generic::list<Connection, xcb_list_fonts_with_info_reply_t, xpp::generic::iterator<Connection,
                                              xcb_fontprop_t,
                                              xpp::generic::signature<decltype(xcb_list_fonts_with_info_properties), xcb_list_fonts_with_info_properties>,
                                              xpp::generic::signature<decltype(xcb_list_fonts_with_info_properties_length), xcb_list_fonts_with_info_properties_length>>>
    properties() {
      return xpp::generic::list<Connection,
                                xcb_list_fonts_with_info_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_fontprop_t,
                                                       xpp::generic::signature<decltype(xcb_list_fonts_with_info_properties), xcb_list_fonts_with_info_properties>,
                                                       xpp::generic::signature<decltype(xcb_list_fonts_with_info_properties_length), xcb_list_fonts_with_info_properties_length>>
                               >(this->m_c, this->get());
    }


    std::string name() {
      return std::string(xcb_list_fonts_with_info_name(this->get().get()), xcb_list_fonts_with_info_name_length(this->get().get()));
    }

};

}

namespace checked {
template<typename Connection>
using list_fonts_with_info = detail::list_fonts_with_info<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_list_fonts_with_info), xcb_list_fonts_with_info>>;
}

namespace unchecked {
template<typename Connection>
using list_fonts_with_info = detail::list_fonts_with_info<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_list_fonts_with_info_unchecked), xcb_list_fonts_with_info_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_fonts_with_info<Connection> list_fonts_with_info(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_fonts_with_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_fonts_with_info<Connection> list_fonts_with_info_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_fonts_with_info<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_font_path_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_set_font_path_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_font_path(Parameter && ... parameter) {
  xcb_set_font_path(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_font_path : public xpp::generic::reply<get_font_path<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_font_path_reply), xcb_get_font_path_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_font_path<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_font_path_reply), xcb_get_font_path_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_font_path(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_get_font_path_reply_t, xpp::generic::iterator<Connection,
                                              xcb_str_t,
                                              xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                              xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                              xpp::generic::signature<decltype(xcb_get_font_path_path_iterator), xcb_get_font_path_path_iterator>>>
    path() {
      return xpp::generic::list<Connection,
                                xcb_get_font_path_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_str_t,
                                                       xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                                       xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_get_font_path_path_iterator), xcb_get_font_path_path_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_font_path = detail::get_font_path<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_font_path), xcb_get_font_path>>;
}

namespace unchecked {
template<typename Connection>
using get_font_path = detail::get_font_path<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_font_path_unchecked), xcb_get_font_path_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_font_path<Connection> get_font_path(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_font_path<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_font_path<Connection> get_font_path_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_font_path<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_pixmap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_create_pixmap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_pixmap(Parameter && ... parameter) {
  xcb_create_pixmap(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void free_pixmap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_free_pixmap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_pixmap(Parameter && ... parameter) {
  xcb_free_pixmap(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_gc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_create_gc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_gc(Parameter && ... parameter) {
  xcb_create_gc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_gc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_change_gc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_gc(Parameter && ... parameter) {
  xcb_change_gc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void copy_gc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_copy_gc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void copy_gc(Parameter && ... parameter) {
  xcb_copy_gc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_dashes_checked(Connection && c, xcb_gcontext_t gc, uint16_t dash_offset, uint16_t dashes_len, const uint8_t * dashes) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_set_dashes_checked(std::forward<Connection>(c), gc, dash_offset, dashes_len, dashes));
}

template<typename Connection>
void set_dashes(Connection && c, xcb_gcontext_t gc, uint16_t dash_offset, uint16_t dashes_len, const uint8_t * dashes) {
  xcb_set_dashes(std::forward<Connection>(c), gc, dash_offset, dashes_len, dashes);
}

template<typename Connection, typename Dashes_Iterator>
void set_dashes_checked(Connection && c, xcb_gcontext_t gc, uint16_t dash_offset, Dashes_Iterator dashes_begin, Dashes_Iterator dashes_end) {
      typedef typename value_type<Dashes_Iterator, ! std::is_pointer<Dashes_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> dashes =
        { value_iterator<Dashes_Iterator>(dashes_begin), value_iterator<Dashes_Iterator>(dashes_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_set_dashes_checked(std::forward<Connection>(c), gc, dash_offset, static_cast<uint16_t>(dashes.size()), const_cast<const vector_type *>(dashes.data())));
}

template<typename Connection, typename Dashes_Iterator>
void set_dashes(Connection && c, xcb_gcontext_t gc, uint16_t dash_offset, Dashes_Iterator dashes_begin, Dashes_Iterator dashes_end) {
      typedef typename value_type<Dashes_Iterator, ! std::is_pointer<Dashes_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> dashes =
        { value_iterator<Dashes_Iterator>(dashes_begin), value_iterator<Dashes_Iterator>(dashes_end) };
      
  xcb_set_dashes(std::forward<Connection>(c), gc, dash_offset, static_cast<uint16_t>(dashes.size()), const_cast<const vector_type *>(dashes.data()));
}

template<typename Connection>
void set_clip_rectangles_checked(Connection && c, uint8_t ordering, xcb_gcontext_t gc, int16_t clip_x_origin, int16_t clip_y_origin, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_set_clip_rectangles_checked(std::forward<Connection>(c), ordering, gc, clip_x_origin, clip_y_origin, rectangles_len, rectangles));
}

template<typename Connection>
void set_clip_rectangles(Connection && c, uint8_t ordering, xcb_gcontext_t gc, int16_t clip_x_origin, int16_t clip_y_origin, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xcb_set_clip_rectangles(std::forward<Connection>(c), ordering, gc, clip_x_origin, clip_y_origin, rectangles_len, rectangles);
}

template<typename Connection, typename Rectangles_Iterator>
void set_clip_rectangles_checked(Connection && c, uint8_t ordering, xcb_gcontext_t gc, int16_t clip_x_origin, int16_t clip_y_origin, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_set_clip_rectangles_checked(std::forward<Connection>(c), ordering, gc, clip_x_origin, clip_y_origin, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data())));
}

template<typename Connection, typename Rectangles_Iterator>
void set_clip_rectangles(Connection && c, uint8_t ordering, xcb_gcontext_t gc, int16_t clip_x_origin, int16_t clip_y_origin, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xcb_set_clip_rectangles(std::forward<Connection>(c), ordering, gc, clip_x_origin, clip_y_origin, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data()));
}

template<typename Connection, typename ... Parameter>
void free_gc_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_free_gc_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_gc(Parameter && ... parameter) {
  xcb_free_gc(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void clear_area_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_clear_area_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void clear_area(Parameter && ... parameter) {
  xcb_clear_area(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void copy_area_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_copy_area_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void copy_area(Parameter && ... parameter) {
  xcb_copy_area(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void copy_plane_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_copy_plane_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void copy_plane(Parameter && ... parameter) {
  xcb_copy_plane(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void poly_point_checked(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t points_len, const xcb_point_t * points) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_point_checked(std::forward<Connection>(c), coordinate_mode, drawable, gc, points_len, points));
}

template<typename Connection>
void poly_point(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t points_len, const xcb_point_t * points) {
  xcb_poly_point(std::forward<Connection>(c), coordinate_mode, drawable, gc, points_len, points);
}

template<typename Connection, typename Points_Iterator>
void poly_point_checked(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_point_checked(std::forward<Connection>(c), coordinate_mode, drawable, gc, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data())));
}

template<typename Connection, typename Points_Iterator>
void poly_point(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xcb_poly_point(std::forward<Connection>(c), coordinate_mode, drawable, gc, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data()));
}

template<typename Connection>
void poly_line_checked(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t points_len, const xcb_point_t * points) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_line_checked(std::forward<Connection>(c), coordinate_mode, drawable, gc, points_len, points));
}

template<typename Connection>
void poly_line(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t points_len, const xcb_point_t * points) {
  xcb_poly_line(std::forward<Connection>(c), coordinate_mode, drawable, gc, points_len, points);
}

template<typename Connection, typename Points_Iterator>
void poly_line_checked(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_line_checked(std::forward<Connection>(c), coordinate_mode, drawable, gc, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data())));
}

template<typename Connection, typename Points_Iterator>
void poly_line(Connection && c, uint8_t coordinate_mode, xcb_drawable_t drawable, xcb_gcontext_t gc, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xcb_poly_line(std::forward<Connection>(c), coordinate_mode, drawable, gc, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data()));
}

template<typename Connection>
void poly_segment_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t segments_len, const xcb_segment_t * segments) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_segment_checked(std::forward<Connection>(c), drawable, gc, segments_len, segments));
}

template<typename Connection>
void poly_segment(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t segments_len, const xcb_segment_t * segments) {
  xcb_poly_segment(std::forward<Connection>(c), drawable, gc, segments_len, segments);
}

template<typename Connection, typename Segments_Iterator>
void poly_segment_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Segments_Iterator segments_begin, Segments_Iterator segments_end) {
      typedef typename value_type<Segments_Iterator, ! std::is_pointer<Segments_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> segments =
        { value_iterator<Segments_Iterator>(segments_begin), value_iterator<Segments_Iterator>(segments_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_segment_checked(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(segments.size()), const_cast<const vector_type *>(segments.data())));
}

template<typename Connection, typename Segments_Iterator>
void poly_segment(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Segments_Iterator segments_begin, Segments_Iterator segments_end) {
      typedef typename value_type<Segments_Iterator, ! std::is_pointer<Segments_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> segments =
        { value_iterator<Segments_Iterator>(segments_begin), value_iterator<Segments_Iterator>(segments_end) };
      
  xcb_poly_segment(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(segments.size()), const_cast<const vector_type *>(segments.data()));
}

template<typename Connection>
void poly_rectangle_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_rectangle_checked(std::forward<Connection>(c), drawable, gc, rectangles_len, rectangles));
}

template<typename Connection>
void poly_rectangle(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xcb_poly_rectangle(std::forward<Connection>(c), drawable, gc, rectangles_len, rectangles);
}

template<typename Connection, typename Rectangles_Iterator>
void poly_rectangle_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_rectangle_checked(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data())));
}

template<typename Connection, typename Rectangles_Iterator>
void poly_rectangle(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xcb_poly_rectangle(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data()));
}

template<typename Connection>
void poly_arc_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t arcs_len, const xcb_arc_t * arcs) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_arc_checked(std::forward<Connection>(c), drawable, gc, arcs_len, arcs));
}

template<typename Connection>
void poly_arc(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t arcs_len, const xcb_arc_t * arcs) {
  xcb_poly_arc(std::forward<Connection>(c), drawable, gc, arcs_len, arcs);
}

template<typename Connection, typename Arcs_Iterator>
void poly_arc_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Arcs_Iterator arcs_begin, Arcs_Iterator arcs_end) {
      typedef typename value_type<Arcs_Iterator, ! std::is_pointer<Arcs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> arcs =
        { value_iterator<Arcs_Iterator>(arcs_begin), value_iterator<Arcs_Iterator>(arcs_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_arc_checked(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(arcs.size()), const_cast<const vector_type *>(arcs.data())));
}

template<typename Connection, typename Arcs_Iterator>
void poly_arc(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Arcs_Iterator arcs_begin, Arcs_Iterator arcs_end) {
      typedef typename value_type<Arcs_Iterator, ! std::is_pointer<Arcs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> arcs =
        { value_iterator<Arcs_Iterator>(arcs_begin), value_iterator<Arcs_Iterator>(arcs_end) };
      
  xcb_poly_arc(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(arcs.size()), const_cast<const vector_type *>(arcs.data()));
}

template<typename Connection>
void fill_poly_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint8_t shape, uint8_t coordinate_mode, uint32_t points_len, const xcb_point_t * points) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_fill_poly_checked(std::forward<Connection>(c), drawable, gc, shape, coordinate_mode, points_len, points));
}

template<typename Connection>
void fill_poly(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint8_t shape, uint8_t coordinate_mode, uint32_t points_len, const xcb_point_t * points) {
  xcb_fill_poly(std::forward<Connection>(c), drawable, gc, shape, coordinate_mode, points_len, points);
}

template<typename Connection, typename Points_Iterator>
void fill_poly_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint8_t shape, uint8_t coordinate_mode, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_fill_poly_checked(std::forward<Connection>(c), drawable, gc, shape, coordinate_mode, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data())));
}

template<typename Connection, typename Points_Iterator>
void fill_poly(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint8_t shape, uint8_t coordinate_mode, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xcb_fill_poly(std::forward<Connection>(c), drawable, gc, shape, coordinate_mode, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data()));
}

template<typename Connection>
void poly_fill_rectangle_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_fill_rectangle_checked(std::forward<Connection>(c), drawable, gc, rectangles_len, rectangles));
}

template<typename Connection>
void poly_fill_rectangle(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xcb_poly_fill_rectangle(std::forward<Connection>(c), drawable, gc, rectangles_len, rectangles);
}

template<typename Connection, typename Rectangles_Iterator>
void poly_fill_rectangle_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_fill_rectangle_checked(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data())));
}

template<typename Connection, typename Rectangles_Iterator>
void poly_fill_rectangle(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xcb_poly_fill_rectangle(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data()));
}

template<typename Connection>
void poly_fill_arc_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t arcs_len, const xcb_arc_t * arcs) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_fill_arc_checked(std::forward<Connection>(c), drawable, gc, arcs_len, arcs));
}

template<typename Connection>
void poly_fill_arc(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, uint32_t arcs_len, const xcb_arc_t * arcs) {
  xcb_poly_fill_arc(std::forward<Connection>(c), drawable, gc, arcs_len, arcs);
}

template<typename Connection, typename Arcs_Iterator>
void poly_fill_arc_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Arcs_Iterator arcs_begin, Arcs_Iterator arcs_end) {
      typedef typename value_type<Arcs_Iterator, ! std::is_pointer<Arcs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> arcs =
        { value_iterator<Arcs_Iterator>(arcs_begin), value_iterator<Arcs_Iterator>(arcs_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_fill_arc_checked(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(arcs.size()), const_cast<const vector_type *>(arcs.data())));
}

template<typename Connection, typename Arcs_Iterator>
void poly_fill_arc(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, Arcs_Iterator arcs_begin, Arcs_Iterator arcs_end) {
      typedef typename value_type<Arcs_Iterator, ! std::is_pointer<Arcs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> arcs =
        { value_iterator<Arcs_Iterator>(arcs_begin), value_iterator<Arcs_Iterator>(arcs_end) };
      
  xcb_poly_fill_arc(std::forward<Connection>(c), drawable, gc, static_cast<uint32_t>(arcs.size()), const_cast<const vector_type *>(arcs.data()));
}

template<typename Connection>
void put_image_checked(Connection && c, uint8_t format, xcb_drawable_t drawable, xcb_gcontext_t gc, uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y, uint8_t left_pad, uint8_t depth, uint32_t data_len, const uint8_t * data) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_put_image_checked(std::forward<Connection>(c), format, drawable, gc, width, height, dst_x, dst_y, left_pad, depth, data_len, data));
}

template<typename Connection>
void put_image(Connection && c, uint8_t format, xcb_drawable_t drawable, xcb_gcontext_t gc, uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y, uint8_t left_pad, uint8_t depth, uint32_t data_len, const uint8_t * data) {
  xcb_put_image(std::forward<Connection>(c), format, drawable, gc, width, height, dst_x, dst_y, left_pad, depth, data_len, data);
}

template<typename Connection, typename Data_Iterator>
void put_image_checked(Connection && c, uint8_t format, xcb_drawable_t drawable, xcb_gcontext_t gc, uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y, uint8_t left_pad, uint8_t depth, Data_Iterator data_begin, Data_Iterator data_end) {
      typedef typename value_type<Data_Iterator, ! std::is_pointer<Data_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> data =
        { value_iterator<Data_Iterator>(data_begin), value_iterator<Data_Iterator>(data_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_put_image_checked(std::forward<Connection>(c), format, drawable, gc, width, height, dst_x, dst_y, left_pad, depth, static_cast<uint32_t>(data.size()), const_cast<const vector_type *>(data.data())));
}

template<typename Connection, typename Data_Iterator>
void put_image(Connection && c, uint8_t format, xcb_drawable_t drawable, xcb_gcontext_t gc, uint16_t width, uint16_t height, int16_t dst_x, int16_t dst_y, uint8_t left_pad, uint8_t depth, Data_Iterator data_begin, Data_Iterator data_end) {
      typedef typename value_type<Data_Iterator, ! std::is_pointer<Data_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> data =
        { value_iterator<Data_Iterator>(data_begin), value_iterator<Data_Iterator>(data_end) };
      
  xcb_put_image(std::forward<Connection>(c), format, drawable, gc, width, height, dst_x, dst_y, left_pad, depth, static_cast<uint32_t>(data.size()), const_cast<const vector_type *>(data.data()));
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_image : public xpp::generic::reply<get_image<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_image_reply), xcb_get_image_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_image<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_image_reply), xcb_get_image_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_image(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_get_image_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_get_image_data), xcb_get_image_data>,
                                              xpp::generic::signature<decltype(xcb_get_image_data_length), xcb_get_image_data_length>>>
    data() {
      return xpp::generic::list<Connection,
                                xcb_get_image_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_get_image_data), xcb_get_image_data>,
                                                       xpp::generic::signature<decltype(xcb_get_image_data_length), xcb_get_image_data_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_image = detail::get_image<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_image), xcb_get_image>>;
}

namespace unchecked {
template<typename Connection>
using get_image = detail::get_image<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_image_unchecked), xcb_get_image_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_image<Connection> get_image(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_image<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_image<Connection> get_image_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_image<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void poly_text_8_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, uint32_t items_len, const uint8_t * items) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_text_8_checked(std::forward<Connection>(c), drawable, gc, x, y, items_len, items));
}

template<typename Connection>
void poly_text_8(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, uint32_t items_len, const uint8_t * items) {
  xcb_poly_text_8(std::forward<Connection>(c), drawable, gc, x, y, items_len, items);
}

template<typename Connection, typename Items_Iterator>
void poly_text_8_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, Items_Iterator items_begin, Items_Iterator items_end) {
      typedef typename value_type<Items_Iterator, ! std::is_pointer<Items_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> items =
        { value_iterator<Items_Iterator>(items_begin), value_iterator<Items_Iterator>(items_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_text_8_checked(std::forward<Connection>(c), drawable, gc, x, y, static_cast<uint32_t>(items.size()), const_cast<const vector_type *>(items.data())));
}

template<typename Connection, typename Items_Iterator>
void poly_text_8(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, Items_Iterator items_begin, Items_Iterator items_end) {
      typedef typename value_type<Items_Iterator, ! std::is_pointer<Items_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> items =
        { value_iterator<Items_Iterator>(items_begin), value_iterator<Items_Iterator>(items_end) };
      
  xcb_poly_text_8(std::forward<Connection>(c), drawable, gc, x, y, static_cast<uint32_t>(items.size()), const_cast<const vector_type *>(items.data()));
}

template<typename Connection>
void poly_text_16_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, uint32_t items_len, const uint8_t * items) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_text_16_checked(std::forward<Connection>(c), drawable, gc, x, y, items_len, items));
}

template<typename Connection>
void poly_text_16(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, uint32_t items_len, const uint8_t * items) {
  xcb_poly_text_16(std::forward<Connection>(c), drawable, gc, x, y, items_len, items);
}

template<typename Connection, typename Items_Iterator>
void poly_text_16_checked(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, Items_Iterator items_begin, Items_Iterator items_end) {
      typedef typename value_type<Items_Iterator, ! std::is_pointer<Items_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> items =
        { value_iterator<Items_Iterator>(items_begin), value_iterator<Items_Iterator>(items_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_poly_text_16_checked(std::forward<Connection>(c), drawable, gc, x, y, static_cast<uint32_t>(items.size()), const_cast<const vector_type *>(items.data())));
}

template<typename Connection, typename Items_Iterator>
void poly_text_16(Connection && c, xcb_drawable_t drawable, xcb_gcontext_t gc, int16_t x, int16_t y, Items_Iterator items_begin, Items_Iterator items_end) {
      typedef typename value_type<Items_Iterator, ! std::is_pointer<Items_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> items =
        { value_iterator<Items_Iterator>(items_begin), value_iterator<Items_Iterator>(items_end) };
      
  xcb_poly_text_16(std::forward<Connection>(c), drawable, gc, x, y, static_cast<uint32_t>(items.size()), const_cast<const vector_type *>(items.data()));
}

template<typename Connection, typename ... Parameter>
void image_text_8_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_image_text_8_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void image_text_8(Parameter && ... parameter) {
  xcb_image_text_8(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void image_text_16_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_image_text_16_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void image_text_16(Parameter && ... parameter) {
  xcb_image_text_16(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_colormap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_create_colormap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_colormap(Parameter && ... parameter) {
  xcb_create_colormap(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void free_colormap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_free_colormap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_colormap(Parameter && ... parameter) {
  xcb_free_colormap(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void copy_colormap_and_free_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_copy_colormap_and_free_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void copy_colormap_and_free(Parameter && ... parameter) {
  xcb_copy_colormap_and_free(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void install_colormap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_install_colormap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void install_colormap(Parameter && ... parameter) {
  xcb_install_colormap(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void uninstall_colormap_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_uninstall_colormap_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void uninstall_colormap(Parameter && ... parameter) {
  xcb_uninstall_colormap(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_installed_colormaps : public xpp::generic::reply<list_installed_colormaps<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_list_installed_colormaps_reply), xcb_list_installed_colormaps_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_installed_colormaps<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_list_installed_colormaps_reply), xcb_list_installed_colormaps_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_installed_colormaps(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    template<typename Cmaps = xcb_colormap_t>
    xpp::generic::list<Connection, xcb_list_installed_colormaps_reply_t, xpp::generic::iterator<Connection,
                                              Cmaps,
                                              xpp::generic::signature<decltype(xcb_list_installed_colormaps_cmaps), xcb_list_installed_colormaps_cmaps>,
                                              xpp::generic::signature<decltype(xcb_list_installed_colormaps_cmaps_length), xcb_list_installed_colormaps_cmaps_length>>>
    cmaps() {
      return xpp::generic::list<Connection,
                                xcb_list_installed_colormaps_reply_t,
                                xpp::generic::iterator<Connection,
                                                       Cmaps,
                                                       xpp::generic::signature<decltype(xcb_list_installed_colormaps_cmaps), xcb_list_installed_colormaps_cmaps>,
                                                       xpp::generic::signature<decltype(xcb_list_installed_colormaps_cmaps_length), xcb_list_installed_colormaps_cmaps_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_installed_colormaps = detail::list_installed_colormaps<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_list_installed_colormaps), xcb_list_installed_colormaps>>;
}

namespace unchecked {
template<typename Connection>
using list_installed_colormaps = detail::list_installed_colormaps<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_list_installed_colormaps_unchecked), xcb_list_installed_colormaps_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_installed_colormaps<Connection> list_installed_colormaps(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_installed_colormaps<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_installed_colormaps<Connection> list_installed_colormaps_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_installed_colormaps<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class alloc_color : public xpp::generic::reply<alloc_color<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_alloc_color_reply), xcb_alloc_color_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<alloc_color<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_alloc_color_reply), xcb_alloc_color_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    alloc_color(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using alloc_color = detail::alloc_color<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_alloc_color), xcb_alloc_color>>;
}

namespace unchecked {
template<typename Connection>
using alloc_color = detail::alloc_color<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_alloc_color_unchecked), xcb_alloc_color_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::alloc_color<Connection> alloc_color(Connection && c, Parameter && ... parameter) {
  return reply::checked::alloc_color<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::alloc_color<Connection> alloc_color_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::alloc_color<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class alloc_named_color : public xpp::generic::reply<alloc_named_color<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_alloc_named_color_reply), xcb_alloc_named_color_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<alloc_named_color<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_alloc_named_color_reply), xcb_alloc_named_color_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    alloc_named_color(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_alloc_named_color_cookie_t cookie(xcb_connection_t * const c, xcb_colormap_t cmap, uint16_t name_len, const char * name) {
      return base::cookie(c, cmap, name_len, name);
    }

    static xcb_alloc_named_color_cookie_t cookie(xcb_connection_t * const c, xcb_colormap_t cmap, const std::string & name) {
      return base::cookie(c, cmap, static_cast<uint16_t>(name.length()), name.c_str());
    }
};

}

namespace checked {
template<typename Connection>
using alloc_named_color = detail::alloc_named_color<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_alloc_named_color), xcb_alloc_named_color>>;
}

namespace unchecked {
template<typename Connection>
using alloc_named_color = detail::alloc_named_color<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_alloc_named_color_unchecked), xcb_alloc_named_color_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::alloc_named_color<Connection> alloc_named_color(Connection && c, Parameter && ... parameter) {
  return reply::checked::alloc_named_color<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::alloc_named_color<Connection> alloc_named_color_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::alloc_named_color<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class alloc_color_cells : public xpp::generic::reply<alloc_color_cells<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_alloc_color_cells_reply), xcb_alloc_color_cells_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<alloc_color_cells<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_alloc_color_cells_reply), xcb_alloc_color_cells_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    alloc_color_cells(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_alloc_color_cells_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_alloc_color_cells_pixels), xcb_alloc_color_cells_pixels>,
                                              xpp::generic::signature<decltype(xcb_alloc_color_cells_pixels_length), xcb_alloc_color_cells_pixels_length>>>
    pixels() {
      return xpp::generic::list<Connection,
                                xcb_alloc_color_cells_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_alloc_color_cells_pixels), xcb_alloc_color_cells_pixels>,
                                                       xpp::generic::signature<decltype(xcb_alloc_color_cells_pixels_length), xcb_alloc_color_cells_pixels_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_alloc_color_cells_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_alloc_color_cells_masks), xcb_alloc_color_cells_masks>,
                                              xpp::generic::signature<decltype(xcb_alloc_color_cells_masks_length), xcb_alloc_color_cells_masks_length>>>
    masks() {
      return xpp::generic::list<Connection,
                                xcb_alloc_color_cells_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_alloc_color_cells_masks), xcb_alloc_color_cells_masks>,
                                                       xpp::generic::signature<decltype(xcb_alloc_color_cells_masks_length), xcb_alloc_color_cells_masks_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using alloc_color_cells = detail::alloc_color_cells<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_alloc_color_cells), xcb_alloc_color_cells>>;
}

namespace unchecked {
template<typename Connection>
using alloc_color_cells = detail::alloc_color_cells<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_alloc_color_cells_unchecked), xcb_alloc_color_cells_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::alloc_color_cells<Connection> alloc_color_cells(Connection && c, Parameter && ... parameter) {
  return reply::checked::alloc_color_cells<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::alloc_color_cells<Connection> alloc_color_cells_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::alloc_color_cells<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class alloc_color_planes : public xpp::generic::reply<alloc_color_planes<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_alloc_color_planes_reply), xcb_alloc_color_planes_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<alloc_color_planes<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_alloc_color_planes_reply), xcb_alloc_color_planes_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    alloc_color_planes(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_alloc_color_planes_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_alloc_color_planes_pixels), xcb_alloc_color_planes_pixels>,
                                              xpp::generic::signature<decltype(xcb_alloc_color_planes_pixels_length), xcb_alloc_color_planes_pixels_length>>>
    pixels() {
      return xpp::generic::list<Connection,
                                xcb_alloc_color_planes_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_alloc_color_planes_pixels), xcb_alloc_color_planes_pixels>,
                                                       xpp::generic::signature<decltype(xcb_alloc_color_planes_pixels_length), xcb_alloc_color_planes_pixels_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using alloc_color_planes = detail::alloc_color_planes<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_alloc_color_planes), xcb_alloc_color_planes>>;
}

namespace unchecked {
template<typename Connection>
using alloc_color_planes = detail::alloc_color_planes<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_alloc_color_planes_unchecked), xcb_alloc_color_planes_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::alloc_color_planes<Connection> alloc_color_planes(Connection && c, Parameter && ... parameter) {
  return reply::checked::alloc_color_planes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::alloc_color_planes<Connection> alloc_color_planes_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::alloc_color_planes<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void free_colors_checked(Connection && c, xcb_colormap_t cmap, uint32_t plane_mask, uint32_t pixels_len, const uint32_t * pixels) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_free_colors_checked(std::forward<Connection>(c), cmap, plane_mask, pixels_len, pixels));
}

template<typename Connection>
void free_colors(Connection && c, xcb_colormap_t cmap, uint32_t plane_mask, uint32_t pixels_len, const uint32_t * pixels) {
  xcb_free_colors(std::forward<Connection>(c), cmap, plane_mask, pixels_len, pixels);
}

template<typename Connection, typename Pixels_Iterator>
void free_colors_checked(Connection && c, xcb_colormap_t cmap, uint32_t plane_mask, Pixels_Iterator pixels_begin, Pixels_Iterator pixels_end) {
      typedef typename value_type<Pixels_Iterator, ! std::is_pointer<Pixels_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> pixels =
        { value_iterator<Pixels_Iterator>(pixels_begin), value_iterator<Pixels_Iterator>(pixels_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_free_colors_checked(std::forward<Connection>(c), cmap, plane_mask, static_cast<uint32_t>(pixels.size()), const_cast<const vector_type *>(pixels.data())));
}

template<typename Connection, typename Pixels_Iterator>
void free_colors(Connection && c, xcb_colormap_t cmap, uint32_t plane_mask, Pixels_Iterator pixels_begin, Pixels_Iterator pixels_end) {
      typedef typename value_type<Pixels_Iterator, ! std::is_pointer<Pixels_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> pixels =
        { value_iterator<Pixels_Iterator>(pixels_begin), value_iterator<Pixels_Iterator>(pixels_end) };
      
  xcb_free_colors(std::forward<Connection>(c), cmap, plane_mask, static_cast<uint32_t>(pixels.size()), const_cast<const vector_type *>(pixels.data()));
}

template<typename Connection>
void store_colors_checked(Connection && c, xcb_colormap_t cmap, uint32_t items_len, const xcb_coloritem_t * items) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_store_colors_checked(std::forward<Connection>(c), cmap, items_len, items));
}

template<typename Connection>
void store_colors(Connection && c, xcb_colormap_t cmap, uint32_t items_len, const xcb_coloritem_t * items) {
  xcb_store_colors(std::forward<Connection>(c), cmap, items_len, items);
}

template<typename Connection, typename Items_Iterator>
void store_colors_checked(Connection && c, xcb_colormap_t cmap, Items_Iterator items_begin, Items_Iterator items_end) {
      typedef typename value_type<Items_Iterator, ! std::is_pointer<Items_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> items =
        { value_iterator<Items_Iterator>(items_begin), value_iterator<Items_Iterator>(items_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_store_colors_checked(std::forward<Connection>(c), cmap, static_cast<uint32_t>(items.size()), const_cast<const vector_type *>(items.data())));
}

template<typename Connection, typename Items_Iterator>
void store_colors(Connection && c, xcb_colormap_t cmap, Items_Iterator items_begin, Items_Iterator items_end) {
      typedef typename value_type<Items_Iterator, ! std::is_pointer<Items_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> items =
        { value_iterator<Items_Iterator>(items_begin), value_iterator<Items_Iterator>(items_end) };
      
  xcb_store_colors(std::forward<Connection>(c), cmap, static_cast<uint32_t>(items.size()), const_cast<const vector_type *>(items.data()));
}

template<typename Connection>
void store_named_color_checked(Connection && c, uint8_t flags, xcb_colormap_t cmap, uint32_t pixel, uint16_t name_len, const char * name) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_store_named_color_checked(std::forward<Connection>(c), flags, cmap, pixel, name_len, name));
}

template<typename Connection>
void store_named_color(Connection && c, uint8_t flags, xcb_colormap_t cmap, uint32_t pixel, uint16_t name_len, const char * name) {
  xcb_store_named_color(std::forward<Connection>(c), flags, cmap, pixel, name_len, name);
}

template<typename Connection>
void store_named_color_checked(Connection && c, uint8_t flags, xcb_colormap_t cmap, uint32_t pixel, const std::string & name) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_store_named_color_checked(std::forward<Connection>(c), flags, cmap, pixel, static_cast<uint16_t>(name.length()), name.c_str()));
}

template<typename Connection>
void store_named_color(Connection && c, uint8_t flags, xcb_colormap_t cmap, uint32_t pixel, const std::string & name) {
  xcb_store_named_color(std::forward<Connection>(c), flags, cmap, pixel, static_cast<uint16_t>(name.length()), name.c_str());
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_colors : public xpp::generic::reply<query_colors<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_colors_reply), xcb_query_colors_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_colors<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_colors_reply), xcb_query_colors_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_colors(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_query_colors_cookie_t cookie(xcb_connection_t * const c, xcb_colormap_t cmap, uint32_t pixels_len, const uint32_t * pixels) {
      return base::cookie(c, cmap, pixels_len, pixels);
    }

    template<typename Pixels_Iterator>
    static xcb_query_colors_cookie_t cookie(xcb_connection_t * const c, xcb_colormap_t cmap, Pixels_Iterator pixels_begin, Pixels_Iterator pixels_end) {
      typedef typename value_type<Pixels_Iterator, ! std::is_pointer<Pixels_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> pixels =
        { value_iterator<Pixels_Iterator>(pixels_begin), value_iterator<Pixels_Iterator>(pixels_end) };
      
      return base::cookie(c, cmap, static_cast<uint32_t>(pixels.size()), const_cast<const vector_type *>(pixels.data()));
    }

    xpp::generic::list<Connection, xcb_query_colors_reply_t, xpp::generic::iterator<Connection,
                                              xcb_rgb_t,
                                              xpp::generic::signature<decltype(xcb_query_colors_colors), xcb_query_colors_colors>,
                                              xpp::generic::signature<decltype(xcb_query_colors_colors_length), xcb_query_colors_colors_length>>>
    colors() {
      return xpp::generic::list<Connection,
                                xcb_query_colors_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_rgb_t,
                                                       xpp::generic::signature<decltype(xcb_query_colors_colors), xcb_query_colors_colors>,
                                                       xpp::generic::signature<decltype(xcb_query_colors_colors_length), xcb_query_colors_colors_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_colors = detail::query_colors<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_colors), xcb_query_colors>>;
}

namespace unchecked {
template<typename Connection>
using query_colors = detail::query_colors<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_colors_unchecked), xcb_query_colors_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_colors<Connection> query_colors(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_colors<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_colors<Connection> query_colors_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_colors<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class lookup_color : public xpp::generic::reply<lookup_color<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_lookup_color_reply), xcb_lookup_color_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<lookup_color<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_lookup_color_reply), xcb_lookup_color_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    lookup_color(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_lookup_color_cookie_t cookie(xcb_connection_t * const c, xcb_colormap_t cmap, uint16_t name_len, const char * name) {
      return base::cookie(c, cmap, name_len, name);
    }

    static xcb_lookup_color_cookie_t cookie(xcb_connection_t * const c, xcb_colormap_t cmap, const std::string & name) {
      return base::cookie(c, cmap, static_cast<uint16_t>(name.length()), name.c_str());
    }
};

}

namespace checked {
template<typename Connection>
using lookup_color = detail::lookup_color<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_lookup_color), xcb_lookup_color>>;
}

namespace unchecked {
template<typename Connection>
using lookup_color = detail::lookup_color<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_lookup_color_unchecked), xcb_lookup_color_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::lookup_color<Connection> lookup_color(Connection && c, Parameter && ... parameter) {
  return reply::checked::lookup_color<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::lookup_color<Connection> lookup_color_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::lookup_color<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_cursor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_create_cursor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_cursor(Parameter && ... parameter) {
  xcb_create_cursor(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_glyph_cursor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_create_glyph_cursor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_glyph_cursor(Parameter && ... parameter) {
  xcb_create_glyph_cursor(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void free_cursor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_free_cursor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_cursor(Parameter && ... parameter) {
  xcb_free_cursor(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void recolor_cursor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_recolor_cursor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void recolor_cursor(Parameter && ... parameter) {
  xcb_recolor_cursor(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_best_size : public xpp::generic::reply<query_best_size<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_best_size_reply), xcb_query_best_size_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_best_size<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_best_size_reply), xcb_query_best_size_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_best_size(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_best_size = detail::query_best_size<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_best_size), xcb_query_best_size>>;
}

namespace unchecked {
template<typename Connection>
using query_best_size = detail::query_best_size<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_best_size_unchecked), xcb_query_best_size_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_best_size<Connection> query_best_size(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_best_size<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_best_size<Connection> query_best_size_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_best_size<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_extension : public xpp::generic::reply<query_extension<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_query_extension_reply), xcb_query_extension_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_extension<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_query_extension_reply), xcb_query_extension_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_extension(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_query_extension_cookie_t cookie(xcb_connection_t * const c, uint16_t name_len, const char * name) {
      return base::cookie(c, name_len, name);
    }

    static xcb_query_extension_cookie_t cookie(xcb_connection_t * const c, const std::string & name) {
      return base::cookie(c, static_cast<uint16_t>(name.length()), name.c_str());
    }
};

}

namespace checked {
template<typename Connection>
using query_extension = detail::query_extension<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_query_extension), xcb_query_extension>>;
}

namespace unchecked {
template<typename Connection>
using query_extension = detail::query_extension<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_query_extension_unchecked), xcb_query_extension_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_extension<Connection> query_extension(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_extension<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_extension<Connection> query_extension_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_extension<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_extensions : public xpp::generic::reply<list_extensions<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_list_extensions_reply), xcb_list_extensions_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_extensions<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_list_extensions_reply), xcb_list_extensions_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_extensions(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_list_extensions_reply_t, xpp::generic::iterator<Connection,
                                              xcb_str_t,
                                              xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                              xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                              xpp::generic::signature<decltype(xcb_list_extensions_names_iterator), xcb_list_extensions_names_iterator>>>
    names() {
      return xpp::generic::list<Connection,
                                xcb_list_extensions_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_str_t,
                                                       xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                                       xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_list_extensions_names_iterator), xcb_list_extensions_names_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_extensions = detail::list_extensions<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_list_extensions), xcb_list_extensions>>;
}

namespace unchecked {
template<typename Connection>
using list_extensions = detail::list_extensions<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_list_extensions_unchecked), xcb_list_extensions_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_extensions<Connection> list_extensions(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_extensions<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_extensions<Connection> list_extensions_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_extensions<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_keyboard_mapping_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_change_keyboard_mapping_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_keyboard_mapping(Parameter && ... parameter) {
  xcb_change_keyboard_mapping(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_keyboard_mapping : public xpp::generic::reply<get_keyboard_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_keyboard_mapping_reply), xcb_get_keyboard_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_keyboard_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_keyboard_mapping_reply), xcb_get_keyboard_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_keyboard_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_get_keyboard_mapping_reply_t, xpp::generic::iterator<Connection,
                                              xcb_keysym_t,
                                              xpp::generic::signature<decltype(xcb_get_keyboard_mapping_keysyms), xcb_get_keyboard_mapping_keysyms>,
                                              xpp::generic::signature<decltype(xcb_get_keyboard_mapping_keysyms_length), xcb_get_keyboard_mapping_keysyms_length>>>
    keysyms() {
      return xpp::generic::list<Connection,
                                xcb_get_keyboard_mapping_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_keysym_t,
                                                       xpp::generic::signature<decltype(xcb_get_keyboard_mapping_keysyms), xcb_get_keyboard_mapping_keysyms>,
                                                       xpp::generic::signature<decltype(xcb_get_keyboard_mapping_keysyms_length), xcb_get_keyboard_mapping_keysyms_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_keyboard_mapping = detail::get_keyboard_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_keyboard_mapping), xcb_get_keyboard_mapping>>;
}

namespace unchecked {
template<typename Connection>
using get_keyboard_mapping = detail::get_keyboard_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_keyboard_mapping_unchecked), xcb_get_keyboard_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_keyboard_mapping<Connection> get_keyboard_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_keyboard_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_keyboard_mapping<Connection> get_keyboard_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_keyboard_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_keyboard_control_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_change_keyboard_control_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_keyboard_control(Parameter && ... parameter) {
  xcb_change_keyboard_control(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_keyboard_control : public xpp::generic::reply<get_keyboard_control<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_keyboard_control_reply), xcb_get_keyboard_control_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_keyboard_control<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_keyboard_control_reply), xcb_get_keyboard_control_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_keyboard_control(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_keyboard_control = detail::get_keyboard_control<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_keyboard_control), xcb_get_keyboard_control>>;
}

namespace unchecked {
template<typename Connection>
using get_keyboard_control = detail::get_keyboard_control<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_keyboard_control_unchecked), xcb_get_keyboard_control_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_keyboard_control<Connection> get_keyboard_control(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_keyboard_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_keyboard_control<Connection> get_keyboard_control_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_keyboard_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void bell_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_bell_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void bell(Parameter && ... parameter) {
  xcb_bell(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_pointer_control_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_change_pointer_control_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_pointer_control(Parameter && ... parameter) {
  xcb_change_pointer_control(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_pointer_control : public xpp::generic::reply<get_pointer_control<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_pointer_control_reply), xcb_get_pointer_control_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_pointer_control<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_pointer_control_reply), xcb_get_pointer_control_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_pointer_control(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_pointer_control = detail::get_pointer_control<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_pointer_control), xcb_get_pointer_control>>;
}

namespace unchecked {
template<typename Connection>
using get_pointer_control = detail::get_pointer_control<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_pointer_control_unchecked), xcb_get_pointer_control_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_pointer_control<Connection> get_pointer_control(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_pointer_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_pointer_control<Connection> get_pointer_control_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_pointer_control<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_screen_saver_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_set_screen_saver_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_screen_saver(Parameter && ... parameter) {
  xcb_set_screen_saver(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_screen_saver : public xpp::generic::reply<get_screen_saver<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_screen_saver_reply), xcb_get_screen_saver_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_screen_saver<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_screen_saver_reply), xcb_get_screen_saver_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_screen_saver(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using get_screen_saver = detail::get_screen_saver<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_screen_saver), xcb_get_screen_saver>>;
}

namespace unchecked {
template<typename Connection>
using get_screen_saver = detail::get_screen_saver<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_screen_saver_unchecked), xcb_get_screen_saver_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_screen_saver<Connection> get_screen_saver(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_screen_saver<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_screen_saver<Connection> get_screen_saver_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_screen_saver<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void change_hosts_checked(Connection && c, uint8_t mode, uint8_t family, uint16_t address_len, const uint8_t * address) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_change_hosts_checked(std::forward<Connection>(c), mode, family, address_len, address));
}

template<typename Connection>
void change_hosts(Connection && c, uint8_t mode, uint8_t family, uint16_t address_len, const uint8_t * address) {
  xcb_change_hosts(std::forward<Connection>(c), mode, family, address_len, address);
}

template<typename Connection, typename Address_Iterator>
void change_hosts_checked(Connection && c, uint8_t mode, uint8_t family, Address_Iterator address_begin, Address_Iterator address_end) {
      typedef typename value_type<Address_Iterator, ! std::is_pointer<Address_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> address =
        { value_iterator<Address_Iterator>(address_begin), value_iterator<Address_Iterator>(address_end) };
      
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(std::forward<Connection>(c), xcb_change_hosts_checked(std::forward<Connection>(c), mode, family, static_cast<uint16_t>(address.size()), const_cast<const vector_type *>(address.data())));
}

template<typename Connection, typename Address_Iterator>
void change_hosts(Connection && c, uint8_t mode, uint8_t family, Address_Iterator address_begin, Address_Iterator address_end) {
      typedef typename value_type<Address_Iterator, ! std::is_pointer<Address_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> address =
        { value_iterator<Address_Iterator>(address_begin), value_iterator<Address_Iterator>(address_end) };
      
  xcb_change_hosts(std::forward<Connection>(c), mode, family, static_cast<uint16_t>(address.size()), const_cast<const vector_type *>(address.data()));
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class list_hosts : public xpp::generic::reply<list_hosts<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_list_hosts_reply), xcb_list_hosts_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<list_hosts<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_list_hosts_reply), xcb_list_hosts_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    list_hosts(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_list_hosts_reply_t, xpp::generic::iterator<Connection,
                                              xcb_host_t,
                                              xpp::generic::signature<decltype(xcb_host_next), xcb_host_next>,
                                              xpp::generic::signature<decltype(xcb_host_sizeof), xcb_host_sizeof>,
                                              xpp::generic::signature<decltype(xcb_list_hosts_hosts_iterator), xcb_list_hosts_hosts_iterator>>>
    hosts() {
      return xpp::generic::list<Connection,
                                xcb_list_hosts_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_host_t,
                                                       xpp::generic::signature<decltype(xcb_host_next), xcb_host_next>,
                                                       xpp::generic::signature<decltype(xcb_host_sizeof), xcb_host_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_list_hosts_hosts_iterator), xcb_list_hosts_hosts_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using list_hosts = detail::list_hosts<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_list_hosts), xcb_list_hosts>>;
}

namespace unchecked {
template<typename Connection>
using list_hosts = detail::list_hosts<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_list_hosts_unchecked), xcb_list_hosts_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::list_hosts<Connection> list_hosts(Connection && c, Parameter && ... parameter) {
  return reply::checked::list_hosts<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::list_hosts<Connection> list_hosts_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::list_hosts<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_access_control_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_set_access_control_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_access_control(Parameter && ... parameter) {
  xcb_set_access_control(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_close_down_mode_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_set_close_down_mode_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_close_down_mode(Parameter && ... parameter) {
  xcb_set_close_down_mode(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void kill_client_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_kill_client_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void kill_client(Parameter && ... parameter) {
  xcb_kill_client(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void rotate_properties_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_rotate_properties_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void rotate_properties(Parameter && ... parameter) {
  xcb_rotate_properties(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void force_screen_saver_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_force_screen_saver_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void force_screen_saver(Parameter && ... parameter) {
  xcb_force_screen_saver(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_pointer_mapping : public xpp::generic::reply<set_pointer_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_set_pointer_mapping_reply), xcb_set_pointer_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_pointer_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_set_pointer_mapping_reply), xcb_set_pointer_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_pointer_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

    static xcb_set_pointer_mapping_cookie_t cookie(xcb_connection_t * const c, uint8_t map_len, const uint8_t * map) {
      return base::cookie(c, map_len, map);
    }

    template<typename Map_Iterator>
    static xcb_set_pointer_mapping_cookie_t cookie(xcb_connection_t * const c, Map_Iterator map_begin, Map_Iterator map_end) {
      typedef typename value_type<Map_Iterator, ! std::is_pointer<Map_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> map =
        { value_iterator<Map_Iterator>(map_begin), value_iterator<Map_Iterator>(map_end) };
      
      return base::cookie(c, static_cast<uint8_t>(map.size()), const_cast<const vector_type *>(map.data()));
    }
};

}

namespace checked {
template<typename Connection>
using set_pointer_mapping = detail::set_pointer_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_set_pointer_mapping), xcb_set_pointer_mapping>>;
}

namespace unchecked {
template<typename Connection>
using set_pointer_mapping = detail::set_pointer_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_set_pointer_mapping_unchecked), xcb_set_pointer_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_pointer_mapping<Connection> set_pointer_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_pointer_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_pointer_mapping<Connection> set_pointer_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_pointer_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_pointer_mapping : public xpp::generic::reply<get_pointer_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_pointer_mapping_reply), xcb_get_pointer_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_pointer_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_pointer_mapping_reply), xcb_get_pointer_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_pointer_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_get_pointer_mapping_reply_t, xpp::generic::iterator<Connection,
                                              uint8_t,
                                              xpp::generic::signature<decltype(xcb_get_pointer_mapping_map), xcb_get_pointer_mapping_map>,
                                              xpp::generic::signature<decltype(xcb_get_pointer_mapping_map_length), xcb_get_pointer_mapping_map_length>>>
    map() {
      return xpp::generic::list<Connection,
                                xcb_get_pointer_mapping_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint8_t,
                                                       xpp::generic::signature<decltype(xcb_get_pointer_mapping_map), xcb_get_pointer_mapping_map>,
                                                       xpp::generic::signature<decltype(xcb_get_pointer_mapping_map_length), xcb_get_pointer_mapping_map_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_pointer_mapping = detail::get_pointer_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_pointer_mapping), xcb_get_pointer_mapping>>;
}

namespace unchecked {
template<typename Connection>
using get_pointer_mapping = detail::get_pointer_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_pointer_mapping_unchecked), xcb_get_pointer_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_pointer_mapping<Connection> get_pointer_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_pointer_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_pointer_mapping<Connection> get_pointer_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_pointer_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class set_modifier_mapping : public xpp::generic::reply<set_modifier_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_set_modifier_mapping_reply), xcb_set_modifier_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<set_modifier_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_set_modifier_mapping_reply), xcb_set_modifier_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    set_modifier_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using set_modifier_mapping = detail::set_modifier_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_set_modifier_mapping), xcb_set_modifier_mapping>>;
}

namespace unchecked {
template<typename Connection>
using set_modifier_mapping = detail::set_modifier_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_set_modifier_mapping_unchecked), xcb_set_modifier_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::set_modifier_mapping<Connection> set_modifier_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::set_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::set_modifier_mapping<Connection> set_modifier_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::set_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_modifier_mapping : public xpp::generic::reply<get_modifier_mapping<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_get_modifier_mapping_reply), xcb_get_modifier_mapping_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_modifier_mapping<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_get_modifier_mapping_reply), xcb_get_modifier_mapping_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_modifier_mapping(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_get_modifier_mapping_reply_t, xpp::generic::iterator<Connection,
                                              xcb_keycode_t,
                                              xpp::generic::signature<decltype(xcb_get_modifier_mapping_keycodes), xcb_get_modifier_mapping_keycodes>,
                                              xpp::generic::signature<decltype(xcb_get_modifier_mapping_keycodes_length), xcb_get_modifier_mapping_keycodes_length>>>
    keycodes() {
      return xpp::generic::list<Connection,
                                xcb_get_modifier_mapping_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_keycode_t,
                                                       xpp::generic::signature<decltype(xcb_get_modifier_mapping_keycodes), xcb_get_modifier_mapping_keycodes>,
                                                       xpp::generic::signature<decltype(xcb_get_modifier_mapping_keycodes_length), xcb_get_modifier_mapping_keycodes_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_modifier_mapping = detail::get_modifier_mapping<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_get_modifier_mapping), xcb_get_modifier_mapping>>;
}

namespace unchecked {
template<typename Connection>
using get_modifier_mapping = detail::get_modifier_mapping<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_get_modifier_mapping_unchecked), xcb_get_modifier_mapping_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_modifier_mapping<Connection> get_modifier_mapping(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_modifier_mapping<Connection> get_modifier_mapping_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_modifier_mapping<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void no_operation_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::x::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_no_operation_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void no_operation(Parameter && ... parameter) {
  xcb_no_operation(std::forward<Parameter>(parameter) ...);
}


template<typename Derived, typename Connection>
class atom
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_atom_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~atom() = default;

    template<typename ... Parameter>
    auto get_name(Parameter && ... parameter) const -> reply::checked::get_atom_name<Connection> {
      return xpp::x::get_atom_name(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_name_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_atom_name<Connection> {
      return xpp::x::get_atom_name_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_selection_owner(Parameter && ... parameter) const -> reply::checked::get_selection_owner<Connection> {
      return xpp::x::get_selection_owner(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_selection_owner_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_selection_owner<Connection> {
      return xpp::x::get_selection_owner_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};

template<typename Derived, typename Connection>
class pixmap
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_pixmap_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~pixmap() = default;

    template<typename ... Parameter>
    void free_checked(Parameter && ... parameter) const {
      xpp::x::free_pixmap_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free(Parameter && ... parameter) const {
      xpp::x::free_pixmap(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};




template<typename Derived, typename Connection>
class gcontext
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_gcontext_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~gcontext() = default;

    template<typename ... Parameter>
    void create_checked(Parameter && ... parameter) const {
      xpp::x::create_gc_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create(Parameter && ... parameter) const {
      xpp::x::create_gc(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_checked(Parameter && ... parameter) const {
      xpp::x::change_gc_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change(Parameter && ... parameter) const {
      xpp::x::change_gc(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_checked(Parameter && ... parameter) const {
      xpp::x::copy_gc_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy(Parameter && ... parameter) const {
      xpp::x::copy_gc(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_dashes_checked(Parameter && ... parameter) const {
      xpp::x::set_dashes_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_dashes(Parameter && ... parameter) const {
      xpp::x::set_dashes(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_checked(Parameter && ... parameter) const {
      xpp::x::free_gc_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free(Parameter && ... parameter) const {
      xpp::x::free_gc(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};





template<typename Derived, typename Connection>
class fontable
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_fontable_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~fontable() = default;

    template<typename ... Parameter>
    auto query_font(Parameter && ... parameter) const -> reply::checked::query_font<Connection> {
      return xpp::x::query_font(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_font_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_font<Connection> {
      return xpp::x::query_font_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_text_extents(Parameter && ... parameter) const -> reply::checked::query_text_extents<Connection> {
      return xpp::x::query_text_extents(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_text_extents_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_text_extents<Connection> {
      return xpp::x::query_text_extents_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};



template<typename Derived, typename Connection>
class colormap
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_colormap_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~colormap() = default;

    template<typename ... Parameter>
    void free_checked(Parameter && ... parameter) const {
      xpp::x::free_colormap_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free(Parameter && ... parameter) const {
      xpp::x::free_colormap(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_and_free_checked(Parameter && ... parameter) const {
      xpp::x::copy_colormap_and_free_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_and_free(Parameter && ... parameter) const {
      xpp::x::copy_colormap_and_free(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void install_checked(Parameter && ... parameter) const {
      xpp::x::install_colormap_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void install(Parameter && ... parameter) const {
      xpp::x::install_colormap(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void uninstall_checked(Parameter && ... parameter) const {
      xpp::x::uninstall_colormap_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void uninstall(Parameter && ... parameter) const {
      xpp::x::uninstall_colormap(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto alloc_color(Parameter && ... parameter) const -> reply::checked::alloc_color<Connection> {
      return xpp::x::alloc_color(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto alloc_color_unchecked(Parameter && ... parameter) const -> reply::unchecked::alloc_color<Connection> {
      return xpp::x::alloc_color_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto alloc_named_color(Parameter && ... parameter) const -> reply::checked::alloc_named_color<Connection> {
      return xpp::x::alloc_named_color(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto alloc_named_color_unchecked(Parameter && ... parameter) const -> reply::unchecked::alloc_named_color<Connection> {
      return xpp::x::alloc_named_color_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_colors_checked(Parameter && ... parameter) const {
      xpp::x::free_colors_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_colors(Parameter && ... parameter) const {
      xpp::x::free_colors(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void store_colors_checked(Parameter && ... parameter) const {
      xpp::x::store_colors_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void store_colors(Parameter && ... parameter) const {
      xpp::x::store_colors(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_colors(Parameter && ... parameter) const -> reply::checked::query_colors<Connection> {
      return xpp::x::query_colors(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_colors_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_colors<Connection> {
      return xpp::x::query_colors_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto lookup_color(Parameter && ... parameter) const -> reply::checked::lookup_color<Connection> {
      return xpp::x::lookup_color(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto lookup_color_unchecked(Parameter && ... parameter) const -> reply::unchecked::lookup_color<Connection> {
      return xpp::x::lookup_color_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};

template<typename Derived, typename Connection>
class drawable
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_drawable_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~drawable() = default;

    template<typename ... Parameter>
    auto get_geometry(Parameter && ... parameter) const -> reply::checked::get_geometry<Connection> {
      return xpp::x::get_geometry(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_geometry_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_geometry<Connection> {
      return xpp::x::get_geometry_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_area_checked(Parameter && ... parameter) const {
      xpp::x::copy_area_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_area(Parameter && ... parameter) const {
      xpp::x::copy_area(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_plane_checked(Parameter && ... parameter) const {
      xpp::x::copy_plane_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_plane(Parameter && ... parameter) const {
      xpp::x::copy_plane(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_segment_checked(Parameter && ... parameter) const {
      xpp::x::poly_segment_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_segment(Parameter && ... parameter) const {
      xpp::x::poly_segment(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_rectangle_checked(Parameter && ... parameter) const {
      xpp::x::poly_rectangle_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_rectangle(Parameter && ... parameter) const {
      xpp::x::poly_rectangle(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_arc_checked(Parameter && ... parameter) const {
      xpp::x::poly_arc_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_arc(Parameter && ... parameter) const {
      xpp::x::poly_arc(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void fill_poly_checked(Parameter && ... parameter) const {
      xpp::x::fill_poly_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void fill_poly(Parameter && ... parameter) const {
      xpp::x::fill_poly(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_fill_rectangle_checked(Parameter && ... parameter) const {
      xpp::x::poly_fill_rectangle_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_fill_rectangle(Parameter && ... parameter) const {
      xpp::x::poly_fill_rectangle(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_fill_arc_checked(Parameter && ... parameter) const {
      xpp::x::poly_fill_arc_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_fill_arc(Parameter && ... parameter) const {
      xpp::x::poly_fill_arc(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_text_8_checked(Parameter && ... parameter) const {
      xpp::x::poly_text_8_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_text_8(Parameter && ... parameter) const {
      xpp::x::poly_text_8(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_text_16_checked(Parameter && ... parameter) const {
      xpp::x::poly_text_16_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_text_16(Parameter && ... parameter) const {
      xpp::x::poly_text_16(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};



template<typename Derived, typename Connection>
class window
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_window_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~window() = default;

    template<typename ... Parameter>
    void change_attributes_checked(Parameter && ... parameter) const {
      xpp::x::change_window_attributes_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_attributes(Parameter && ... parameter) const {
      xpp::x::change_window_attributes(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_attributes(Parameter && ... parameter) const -> reply::checked::get_window_attributes<Connection> {
      return xpp::x::get_window_attributes(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_attributes_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_window_attributes<Connection> {
      return xpp::x::get_window_attributes_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::x::destroy_window_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::x::destroy_window(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_subwindows_checked(Parameter && ... parameter) const {
      xpp::x::destroy_subwindows_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_subwindows(Parameter && ... parameter) const {
      xpp::x::destroy_subwindows(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void reparent_checked(Parameter && ... parameter) const {
      xpp::x::reparent_window_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void reparent(Parameter && ... parameter) const {
      xpp::x::reparent_window(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void map_checked(Parameter && ... parameter) const {
      xpp::x::map_window_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void map(Parameter && ... parameter) const {
      xpp::x::map_window(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void map_subwindows_checked(Parameter && ... parameter) const {
      xpp::x::map_subwindows_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void map_subwindows(Parameter && ... parameter) const {
      xpp::x::map_subwindows(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unmap_checked(Parameter && ... parameter) const {
      xpp::x::unmap_window_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unmap(Parameter && ... parameter) const {
      xpp::x::unmap_window(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unmap_subwindows_checked(Parameter && ... parameter) const {
      xpp::x::unmap_subwindows_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unmap_subwindows(Parameter && ... parameter) const {
      xpp::x::unmap_subwindows(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void configure_checked(Parameter && ... parameter) const {
      xpp::x::configure_window_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void configure(Parameter && ... parameter) const {
      xpp::x::configure_window(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_tree(Parameter && ... parameter) const -> reply::checked::query_tree<Connection> {
      return xpp::x::query_tree(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_tree_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_tree<Connection> {
      return xpp::x::query_tree_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_property_checked(Parameter && ... parameter) const {
      xpp::x::delete_property_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_property(Parameter && ... parameter) const {
      xpp::x::delete_property(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_properties(Parameter && ... parameter) const -> reply::checked::list_properties<Connection> {
      return xpp::x::list_properties(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_properties<Connection> {
      return xpp::x::list_properties_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_selection_owner_checked(Parameter && ... parameter) const {
      xpp::x::set_selection_owner_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_selection_owner(Parameter && ... parameter) const {
      xpp::x::set_selection_owner(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void convert_selection_checked(Parameter && ... parameter) const {
      xpp::x::convert_selection_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void convert_selection(Parameter && ... parameter) const {
      xpp::x::convert_selection(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_pointer(Parameter && ... parameter) const -> reply::checked::query_pointer<Connection> {
      return xpp::x::query_pointer(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_pointer_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_pointer<Connection> {
      return xpp::x::query_pointer_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_motion_events(Parameter && ... parameter) const -> reply::checked::get_motion_events<Connection> {
      return xpp::x::get_motion_events(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_motion_events_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_motion_events<Connection> {
      return xpp::x::get_motion_events_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto translate_coordinates(Parameter && ... parameter) const -> reply::checked::translate_coordinates<Connection> {
      return xpp::x::translate_coordinates(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto translate_coordinates_unchecked(Parameter && ... parameter) const -> reply::unchecked::translate_coordinates<Connection> {
      return xpp::x::translate_coordinates_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void warp_pointer_checked(Parameter && ... parameter) const {
      xpp::x::warp_pointer_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void warp_pointer(Parameter && ... parameter) const {
      xpp::x::warp_pointer(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_installed_colormaps(Parameter && ... parameter) const -> reply::checked::list_installed_colormaps<Connection> {
      return xpp::x::list_installed_colormaps(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_installed_colormaps_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_installed_colormaps<Connection> {
      return xpp::x::list_installed_colormaps_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void rotate_properties_checked(Parameter && ... parameter) const {
      xpp::x::rotate_properties_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void rotate_properties(Parameter && ... parameter) const {
      xpp::x::rotate_properties(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};

template<typename Derived, typename Connection>
class font
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_font_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~font() = default;

    template<typename ... Parameter>
    void open_checked(Parameter && ... parameter) const {
      xpp::x::open_font_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void open(Parameter && ... parameter) const {
      xpp::x::open_font(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void close_checked(Parameter && ... parameter) const {
      xpp::x::close_font_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void close(Parameter && ... parameter) const {
      xpp::x::close_font(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};












template<typename Derived, typename Connection>
class cursor
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_cursor_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~cursor() = default;

    template<typename ... Parameter>
    void change_active_pointer_grab_checked(Parameter && ... parameter) const {
      xpp::x::change_active_pointer_grab_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_active_pointer_grab(Parameter && ... parameter) const {
      xpp::x::change_active_pointer_grab(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_checked(Parameter && ... parameter) const {
      xpp::x::create_cursor_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create(Parameter && ... parameter) const {
      xpp::x::create_cursor(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_glyph_checked(Parameter && ... parameter) const {
      xpp::x::create_glyph_cursor_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_glyph(Parameter && ... parameter) const {
      xpp::x::create_glyph_cursor(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_checked(Parameter && ... parameter) const {
      xpp::x::free_cursor_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free(Parameter && ... parameter) const {
      xpp::x::free_cursor(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void recolor_checked(Parameter && ... parameter) const {
      xpp::x::recolor_cursor_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void recolor(Parameter && ... parameter) const {
      xpp::x::recolor_cursor(connection(),
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

    virtual ~interface() = default;

    const interface<Derived, Connection> &
    x() {
      return *this;
    }

    template<typename ... Parameter>
    void create_window_checked(Parameter && ... parameter) const {
      xpp::x::create_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_window(Parameter && ... parameter) const {
      xpp::x::create_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_window_attributes_checked(Parameter && ... parameter) const {
      xpp::x::change_window_attributes_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_window_attributes(Parameter && ... parameter) const {
      xpp::x::change_window_attributes(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_window_attributes(Parameter && ... parameter) const -> reply::checked::get_window_attributes<Connection> {
      return xpp::x::get_window_attributes(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_window_attributes_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_window_attributes<Connection> {
      return xpp::x::get_window_attributes_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_window_checked(Parameter && ... parameter) const {
      xpp::x::destroy_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_window(Parameter && ... parameter) const {
      xpp::x::destroy_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_subwindows_checked(Parameter && ... parameter) const {
      xpp::x::destroy_subwindows_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy_subwindows(Parameter && ... parameter) const {
      xpp::x::destroy_subwindows(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_save_set_checked(Parameter && ... parameter) const {
      xpp::x::change_save_set_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_save_set(Parameter && ... parameter) const {
      xpp::x::change_save_set(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void reparent_window_checked(Parameter && ... parameter) const {
      xpp::x::reparent_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void reparent_window(Parameter && ... parameter) const {
      xpp::x::reparent_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void map_window_checked(Parameter && ... parameter) const {
      xpp::x::map_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void map_window(Parameter && ... parameter) const {
      xpp::x::map_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void map_subwindows_checked(Parameter && ... parameter) const {
      xpp::x::map_subwindows_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void map_subwindows(Parameter && ... parameter) const {
      xpp::x::map_subwindows(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unmap_window_checked(Parameter && ... parameter) const {
      xpp::x::unmap_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unmap_window(Parameter && ... parameter) const {
      xpp::x::unmap_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void unmap_subwindows_checked(Parameter && ... parameter) const {
      xpp::x::unmap_subwindows_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void unmap_subwindows(Parameter && ... parameter) const {
      xpp::x::unmap_subwindows(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void configure_window_checked(Parameter && ... parameter) const {
      xpp::x::configure_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void configure_window(Parameter && ... parameter) const {
      xpp::x::configure_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void circulate_window_checked(Parameter && ... parameter) const {
      xpp::x::circulate_window_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void circulate_window(Parameter && ... parameter) const {
      xpp::x::circulate_window(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_geometry(Parameter && ... parameter) const -> reply::checked::get_geometry<Connection> {
      return xpp::x::get_geometry(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_geometry_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_geometry<Connection> {
      return xpp::x::get_geometry_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_tree(Parameter && ... parameter) const -> reply::checked::query_tree<Connection> {
      return xpp::x::query_tree(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_tree_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_tree<Connection> {
      return xpp::x::query_tree_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto intern_atom(Parameter && ... parameter) const -> reply::checked::intern_atom<Connection> {
      return xpp::x::intern_atom(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto intern_atom_unchecked(Parameter && ... parameter) const -> reply::unchecked::intern_atom<Connection> {
      return xpp::x::intern_atom_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_atom_name(Parameter && ... parameter) const -> reply::checked::get_atom_name<Connection> {
      return xpp::x::get_atom_name(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_atom_name_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_atom_name<Connection> {
      return xpp::x::get_atom_name_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_property_checked(Parameter && ... parameter) const {
      xpp::x::change_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_property(Parameter && ... parameter) const {
      xpp::x::change_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void delete_property_checked(Parameter && ... parameter) const {
      xpp::x::delete_property_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void delete_property(Parameter && ... parameter) const {
      xpp::x::delete_property(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_property(Parameter && ... parameter) const -> reply::checked::get_property<Connection> {
      return xpp::x::get_property(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_property_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_property<Connection> {
      return xpp::x::get_property_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_properties(Parameter && ... parameter) const -> reply::checked::list_properties<Connection> {
      return xpp::x::list_properties(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_properties_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_properties<Connection> {
      return xpp::x::list_properties_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_selection_owner_checked(Parameter && ... parameter) const {
      xpp::x::set_selection_owner_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_selection_owner(Parameter && ... parameter) const {
      xpp::x::set_selection_owner(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_selection_owner(Parameter && ... parameter) const -> reply::checked::get_selection_owner<Connection> {
      return xpp::x::get_selection_owner(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_selection_owner_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_selection_owner<Connection> {
      return xpp::x::get_selection_owner_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void convert_selection_checked(Parameter && ... parameter) const {
      xpp::x::convert_selection_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void convert_selection(Parameter && ... parameter) const {
      xpp::x::convert_selection(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void send_event_checked(Parameter && ... parameter) const {
      xpp::x::send_event_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void send_event(Parameter && ... parameter) const {
      xpp::x::send_event(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto grab_pointer(Parameter && ... parameter) const -> reply::checked::grab_pointer<Connection> {
      return xpp::x::grab_pointer(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto grab_pointer_unchecked(Parameter && ... parameter) const -> reply::unchecked::grab_pointer<Connection> {
      return xpp::x::grab_pointer_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_pointer_checked(Parameter && ... parameter) const {
      xpp::x::ungrab_pointer_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_pointer(Parameter && ... parameter) const {
      xpp::x::ungrab_pointer(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void grab_button_checked(Parameter && ... parameter) const {
      xpp::x::grab_button_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void grab_button(Parameter && ... parameter) const {
      xpp::x::grab_button(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_button_checked(Parameter && ... parameter) const {
      xpp::x::ungrab_button_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_button(Parameter && ... parameter) const {
      xpp::x::ungrab_button(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_active_pointer_grab_checked(Parameter && ... parameter) const {
      xpp::x::change_active_pointer_grab_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_active_pointer_grab(Parameter && ... parameter) const {
      xpp::x::change_active_pointer_grab(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto grab_keyboard(Parameter && ... parameter) const -> reply::checked::grab_keyboard<Connection> {
      return xpp::x::grab_keyboard(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto grab_keyboard_unchecked(Parameter && ... parameter) const -> reply::unchecked::grab_keyboard<Connection> {
      return xpp::x::grab_keyboard_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_keyboard_checked(Parameter && ... parameter) const {
      xpp::x::ungrab_keyboard_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_keyboard(Parameter && ... parameter) const {
      xpp::x::ungrab_keyboard(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void grab_key_checked(Parameter && ... parameter) const {
      xpp::x::grab_key_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void grab_key(Parameter && ... parameter) const {
      xpp::x::grab_key(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_key_checked(Parameter && ... parameter) const {
      xpp::x::ungrab_key_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_key(Parameter && ... parameter) const {
      xpp::x::ungrab_key(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void allow_events_checked(Parameter && ... parameter) const {
      xpp::x::allow_events_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void allow_events(Parameter && ... parameter) const {
      xpp::x::allow_events(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void grab_server_checked(Parameter && ... parameter) const {
      xpp::x::grab_server_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void grab_server(Parameter && ... parameter) const {
      xpp::x::grab_server(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void ungrab_server_checked(Parameter && ... parameter) const {
      xpp::x::ungrab_server_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void ungrab_server(Parameter && ... parameter) const {
      xpp::x::ungrab_server(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_pointer(Parameter && ... parameter) const -> reply::checked::query_pointer<Connection> {
      return xpp::x::query_pointer(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_pointer_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_pointer<Connection> {
      return xpp::x::query_pointer_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_motion_events(Parameter && ... parameter) const -> reply::checked::get_motion_events<Connection> {
      return xpp::x::get_motion_events(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_motion_events_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_motion_events<Connection> {
      return xpp::x::get_motion_events_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto translate_coordinates(Parameter && ... parameter) const -> reply::checked::translate_coordinates<Connection> {
      return xpp::x::translate_coordinates(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto translate_coordinates_unchecked(Parameter && ... parameter) const -> reply::unchecked::translate_coordinates<Connection> {
      return xpp::x::translate_coordinates_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void warp_pointer_checked(Parameter && ... parameter) const {
      xpp::x::warp_pointer_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void warp_pointer(Parameter && ... parameter) const {
      xpp::x::warp_pointer(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_input_focus_checked(Parameter && ... parameter) const {
      xpp::x::set_input_focus_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_input_focus(Parameter && ... parameter) const {
      xpp::x::set_input_focus(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_input_focus(Parameter && ... parameter) const -> reply::checked::get_input_focus<Connection> {
      return xpp::x::get_input_focus(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_input_focus_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_input_focus<Connection> {
      return xpp::x::get_input_focus_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_keymap(Parameter && ... parameter) const -> reply::checked::query_keymap<Connection> {
      return xpp::x::query_keymap(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_keymap_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_keymap<Connection> {
      return xpp::x::query_keymap_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void open_font_checked(Parameter && ... parameter) const {
      xpp::x::open_font_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void open_font(Parameter && ... parameter) const {
      xpp::x::open_font(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void close_font_checked(Parameter && ... parameter) const {
      xpp::x::close_font_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void close_font(Parameter && ... parameter) const {
      xpp::x::close_font(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_font(Parameter && ... parameter) const -> reply::checked::query_font<Connection> {
      return xpp::x::query_font(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_font_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_font<Connection> {
      return xpp::x::query_font_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_text_extents(Parameter && ... parameter) const -> reply::checked::query_text_extents<Connection> {
      return xpp::x::query_text_extents(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_text_extents_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_text_extents<Connection> {
      return xpp::x::query_text_extents_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_fonts(Parameter && ... parameter) const -> reply::checked::list_fonts<Connection> {
      return xpp::x::list_fonts(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_fonts_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_fonts<Connection> {
      return xpp::x::list_fonts_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_fonts_with_info(Parameter && ... parameter) const -> reply::checked::list_fonts_with_info<Connection> {
      return xpp::x::list_fonts_with_info(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_fonts_with_info_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_fonts_with_info<Connection> {
      return xpp::x::list_fonts_with_info_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_font_path_checked(Parameter && ... parameter) const {
      xpp::x::set_font_path_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_font_path(Parameter && ... parameter) const {
      xpp::x::set_font_path(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_font_path(Parameter && ... parameter) const -> reply::checked::get_font_path<Connection> {
      return xpp::x::get_font_path(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_font_path_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_font_path<Connection> {
      return xpp::x::get_font_path_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_pixmap_checked(Parameter && ... parameter) const {
      xpp::x::create_pixmap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_pixmap(Parameter && ... parameter) const {
      xpp::x::create_pixmap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_pixmap_checked(Parameter && ... parameter) const {
      xpp::x::free_pixmap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_pixmap(Parameter && ... parameter) const {
      xpp::x::free_pixmap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_gc_checked(Parameter && ... parameter) const {
      xpp::x::create_gc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_gc(Parameter && ... parameter) const {
      xpp::x::create_gc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_gc_checked(Parameter && ... parameter) const {
      xpp::x::change_gc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_gc(Parameter && ... parameter) const {
      xpp::x::change_gc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_gc_checked(Parameter && ... parameter) const {
      xpp::x::copy_gc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_gc(Parameter && ... parameter) const {
      xpp::x::copy_gc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_dashes_checked(Parameter && ... parameter) const {
      xpp::x::set_dashes_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_dashes(Parameter && ... parameter) const {
      xpp::x::set_dashes(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_clip_rectangles_checked(Parameter && ... parameter) const {
      xpp::x::set_clip_rectangles_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_clip_rectangles(Parameter && ... parameter) const {
      xpp::x::set_clip_rectangles(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_gc_checked(Parameter && ... parameter) const {
      xpp::x::free_gc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_gc(Parameter && ... parameter) const {
      xpp::x::free_gc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void clear_area_checked(Parameter && ... parameter) const {
      xpp::x::clear_area_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void clear_area(Parameter && ... parameter) const {
      xpp::x::clear_area(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_area_checked(Parameter && ... parameter) const {
      xpp::x::copy_area_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_area(Parameter && ... parameter) const {
      xpp::x::copy_area(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_plane_checked(Parameter && ... parameter) const {
      xpp::x::copy_plane_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_plane(Parameter && ... parameter) const {
      xpp::x::copy_plane(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_point_checked(Parameter && ... parameter) const {
      xpp::x::poly_point_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_point(Parameter && ... parameter) const {
      xpp::x::poly_point(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_line_checked(Parameter && ... parameter) const {
      xpp::x::poly_line_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_line(Parameter && ... parameter) const {
      xpp::x::poly_line(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_segment_checked(Parameter && ... parameter) const {
      xpp::x::poly_segment_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_segment(Parameter && ... parameter) const {
      xpp::x::poly_segment(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_rectangle_checked(Parameter && ... parameter) const {
      xpp::x::poly_rectangle_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_rectangle(Parameter && ... parameter) const {
      xpp::x::poly_rectangle(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_arc_checked(Parameter && ... parameter) const {
      xpp::x::poly_arc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_arc(Parameter && ... parameter) const {
      xpp::x::poly_arc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void fill_poly_checked(Parameter && ... parameter) const {
      xpp::x::fill_poly_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void fill_poly(Parameter && ... parameter) const {
      xpp::x::fill_poly(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_fill_rectangle_checked(Parameter && ... parameter) const {
      xpp::x::poly_fill_rectangle_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_fill_rectangle(Parameter && ... parameter) const {
      xpp::x::poly_fill_rectangle(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_fill_arc_checked(Parameter && ... parameter) const {
      xpp::x::poly_fill_arc_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_fill_arc(Parameter && ... parameter) const {
      xpp::x::poly_fill_arc(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void put_image_checked(Parameter && ... parameter) const {
      xpp::x::put_image_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void put_image(Parameter && ... parameter) const {
      xpp::x::put_image(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_image(Parameter && ... parameter) const -> reply::checked::get_image<Connection> {
      return xpp::x::get_image(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_image_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_image<Connection> {
      return xpp::x::get_image_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_text_8_checked(Parameter && ... parameter) const {
      xpp::x::poly_text_8_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_text_8(Parameter && ... parameter) const {
      xpp::x::poly_text_8(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void poly_text_16_checked(Parameter && ... parameter) const {
      xpp::x::poly_text_16_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void poly_text_16(Parameter && ... parameter) const {
      xpp::x::poly_text_16(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void image_text_8_checked(Parameter && ... parameter) const {
      xpp::x::image_text_8_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void image_text_8(Parameter && ... parameter) const {
      xpp::x::image_text_8(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void image_text_16_checked(Parameter && ... parameter) const {
      xpp::x::image_text_16_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void image_text_16(Parameter && ... parameter) const {
      xpp::x::image_text_16(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_colormap_checked(Parameter && ... parameter) const {
      xpp::x::create_colormap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_colormap(Parameter && ... parameter) const {
      xpp::x::create_colormap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_colormap_checked(Parameter && ... parameter) const {
      xpp::x::free_colormap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_colormap(Parameter && ... parameter) const {
      xpp::x::free_colormap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void copy_colormap_and_free_checked(Parameter && ... parameter) const {
      xpp::x::copy_colormap_and_free_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void copy_colormap_and_free(Parameter && ... parameter) const {
      xpp::x::copy_colormap_and_free(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void install_colormap_checked(Parameter && ... parameter) const {
      xpp::x::install_colormap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void install_colormap(Parameter && ... parameter) const {
      xpp::x::install_colormap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void uninstall_colormap_checked(Parameter && ... parameter) const {
      xpp::x::uninstall_colormap_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void uninstall_colormap(Parameter && ... parameter) const {
      xpp::x::uninstall_colormap(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_installed_colormaps(Parameter && ... parameter) const -> reply::checked::list_installed_colormaps<Connection> {
      return xpp::x::list_installed_colormaps(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_installed_colormaps_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_installed_colormaps<Connection> {
      return xpp::x::list_installed_colormaps_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto alloc_color(Parameter && ... parameter) const -> reply::checked::alloc_color<Connection> {
      return xpp::x::alloc_color(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto alloc_color_unchecked(Parameter && ... parameter) const -> reply::unchecked::alloc_color<Connection> {
      return xpp::x::alloc_color_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto alloc_named_color(Parameter && ... parameter) const -> reply::checked::alloc_named_color<Connection> {
      return xpp::x::alloc_named_color(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto alloc_named_color_unchecked(Parameter && ... parameter) const -> reply::unchecked::alloc_named_color<Connection> {
      return xpp::x::alloc_named_color_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto alloc_color_cells(Parameter && ... parameter) const -> reply::checked::alloc_color_cells<Connection> {
      return xpp::x::alloc_color_cells(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto alloc_color_cells_unchecked(Parameter && ... parameter) const -> reply::unchecked::alloc_color_cells<Connection> {
      return xpp::x::alloc_color_cells_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto alloc_color_planes(Parameter && ... parameter) const -> reply::checked::alloc_color_planes<Connection> {
      return xpp::x::alloc_color_planes(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto alloc_color_planes_unchecked(Parameter && ... parameter) const -> reply::unchecked::alloc_color_planes<Connection> {
      return xpp::x::alloc_color_planes_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_colors_checked(Parameter && ... parameter) const {
      xpp::x::free_colors_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_colors(Parameter && ... parameter) const {
      xpp::x::free_colors(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void store_colors_checked(Parameter && ... parameter) const {
      xpp::x::store_colors_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void store_colors(Parameter && ... parameter) const {
      xpp::x::store_colors(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void store_named_color_checked(Parameter && ... parameter) const {
      xpp::x::store_named_color_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void store_named_color(Parameter && ... parameter) const {
      xpp::x::store_named_color(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_colors(Parameter && ... parameter) const -> reply::checked::query_colors<Connection> {
      return xpp::x::query_colors(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_colors_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_colors<Connection> {
      return xpp::x::query_colors_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto lookup_color(Parameter && ... parameter) const -> reply::checked::lookup_color<Connection> {
      return xpp::x::lookup_color(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto lookup_color_unchecked(Parameter && ... parameter) const -> reply::unchecked::lookup_color<Connection> {
      return xpp::x::lookup_color_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_cursor_checked(Parameter && ... parameter) const {
      xpp::x::create_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_cursor(Parameter && ... parameter) const {
      xpp::x::create_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_glyph_cursor_checked(Parameter && ... parameter) const {
      xpp::x::create_glyph_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_glyph_cursor(Parameter && ... parameter) const {
      xpp::x::create_glyph_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_cursor_checked(Parameter && ... parameter) const {
      xpp::x::free_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_cursor(Parameter && ... parameter) const {
      xpp::x::free_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void recolor_cursor_checked(Parameter && ... parameter) const {
      xpp::x::recolor_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void recolor_cursor(Parameter && ... parameter) const {
      xpp::x::recolor_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_best_size(Parameter && ... parameter) const -> reply::checked::query_best_size<Connection> {
      return xpp::x::query_best_size(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_best_size_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_best_size<Connection> {
      return xpp::x::query_best_size_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_extension(Parameter && ... parameter) const -> reply::checked::query_extension<Connection> {
      return xpp::x::query_extension(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_extension_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_extension<Connection> {
      return xpp::x::query_extension_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_extensions(Parameter && ... parameter) const -> reply::checked::list_extensions<Connection> {
      return xpp::x::list_extensions(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_extensions_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_extensions<Connection> {
      return xpp::x::list_extensions_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_keyboard_mapping_checked(Parameter && ... parameter) const {
      xpp::x::change_keyboard_mapping_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_keyboard_mapping(Parameter && ... parameter) const {
      xpp::x::change_keyboard_mapping(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_keyboard_mapping(Parameter && ... parameter) const -> reply::checked::get_keyboard_mapping<Connection> {
      return xpp::x::get_keyboard_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_keyboard_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_keyboard_mapping<Connection> {
      return xpp::x::get_keyboard_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_keyboard_control_checked(Parameter && ... parameter) const {
      xpp::x::change_keyboard_control_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_keyboard_control(Parameter && ... parameter) const {
      xpp::x::change_keyboard_control(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_keyboard_control(Parameter && ... parameter) const -> reply::checked::get_keyboard_control<Connection> {
      return xpp::x::get_keyboard_control(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_keyboard_control_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_keyboard_control<Connection> {
      return xpp::x::get_keyboard_control_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void bell_checked(Parameter && ... parameter) const {
      xpp::x::bell_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void bell(Parameter && ... parameter) const {
      xpp::x::bell(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_pointer_control_checked(Parameter && ... parameter) const {
      xpp::x::change_pointer_control_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_pointer_control(Parameter && ... parameter) const {
      xpp::x::change_pointer_control(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_pointer_control(Parameter && ... parameter) const -> reply::checked::get_pointer_control<Connection> {
      return xpp::x::get_pointer_control(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_pointer_control_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_pointer_control<Connection> {
      return xpp::x::get_pointer_control_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_screen_saver_checked(Parameter && ... parameter) const {
      xpp::x::set_screen_saver_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_screen_saver(Parameter && ... parameter) const {
      xpp::x::set_screen_saver(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_screen_saver(Parameter && ... parameter) const -> reply::checked::get_screen_saver<Connection> {
      return xpp::x::get_screen_saver(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_screen_saver_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_screen_saver<Connection> {
      return xpp::x::get_screen_saver_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_hosts_checked(Parameter && ... parameter) const {
      xpp::x::change_hosts_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_hosts(Parameter && ... parameter) const {
      xpp::x::change_hosts(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto list_hosts(Parameter && ... parameter) const -> reply::checked::list_hosts<Connection> {
      return xpp::x::list_hosts(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto list_hosts_unchecked(Parameter && ... parameter) const -> reply::unchecked::list_hosts<Connection> {
      return xpp::x::list_hosts_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_access_control_checked(Parameter && ... parameter) const {
      xpp::x::set_access_control_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_access_control(Parameter && ... parameter) const {
      xpp::x::set_access_control(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_close_down_mode_checked(Parameter && ... parameter) const {
      xpp::x::set_close_down_mode_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_close_down_mode(Parameter && ... parameter) const {
      xpp::x::set_close_down_mode(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void kill_client_checked(Parameter && ... parameter) const {
      xpp::x::kill_client_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void kill_client(Parameter && ... parameter) const {
      xpp::x::kill_client(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void rotate_properties_checked(Parameter && ... parameter) const {
      xpp::x::rotate_properties_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void rotate_properties(Parameter && ... parameter) const {
      xpp::x::rotate_properties(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void force_screen_saver_checked(Parameter && ... parameter) const {
      xpp::x::force_screen_saver_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void force_screen_saver(Parameter && ... parameter) const {
      xpp::x::force_screen_saver(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_pointer_mapping(Parameter && ... parameter) const -> reply::checked::set_pointer_mapping<Connection> {
      return xpp::x::set_pointer_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_pointer_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_pointer_mapping<Connection> {
      return xpp::x::set_pointer_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_pointer_mapping(Parameter && ... parameter) const -> reply::checked::get_pointer_mapping<Connection> {
      return xpp::x::get_pointer_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_pointer_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_pointer_mapping<Connection> {
      return xpp::x::get_pointer_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto set_modifier_mapping(Parameter && ... parameter) const -> reply::checked::set_modifier_mapping<Connection> {
      return xpp::x::set_modifier_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto set_modifier_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::set_modifier_mapping<Connection> {
      return xpp::x::set_modifier_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_modifier_mapping(Parameter && ... parameter) const -> reply::checked::get_modifier_mapping<Connection> {
      return xpp::x::get_modifier_mapping(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_modifier_mapping_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_modifier_mapping<Connection> {
      return xpp::x::get_modifier_mapping_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void no_operation_checked(Parameter && ... parameter) const {
      xpp::x::no_operation_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void no_operation(Parameter && ... parameter) const {
      xpp::x::no_operation(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::x::extension extension;

    template<typename C>
    dispatcher(C && c) : m_c(std::forward<C>(c))  {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch (event->response_type & ~0x80) {

        case XCB_KEY_PRESS:
          handler(xpp::x::event::key_press<Connection>(m_c, event));
          return true;

        case XCB_KEY_RELEASE:
          handler(xpp::x::event::key_release<Connection>(m_c, event));
          return true;

        case XCB_BUTTON_PRESS:
          handler(xpp::x::event::button_press<Connection>(m_c, event));
          return true;

        case XCB_BUTTON_RELEASE:
          handler(xpp::x::event::button_release<Connection>(m_c, event));
          return true;

        case XCB_MOTION_NOTIFY:
          handler(xpp::x::event::motion_notify<Connection>(m_c, event));
          return true;

        case XCB_ENTER_NOTIFY:
          handler(xpp::x::event::enter_notify<Connection>(m_c, event));
          return true;

        case XCB_LEAVE_NOTIFY:
          handler(xpp::x::event::leave_notify<Connection>(m_c, event));
          return true;

        case XCB_FOCUS_IN:
          handler(xpp::x::event::focus_in<Connection>(m_c, event));
          return true;

        case XCB_FOCUS_OUT:
          handler(xpp::x::event::focus_out<Connection>(m_c, event));
          return true;

        case XCB_KEYMAP_NOTIFY:
          handler(xpp::x::event::keymap_notify<Connection>(m_c, event));
          return true;

        case XCB_EXPOSE:
          handler(xpp::x::event::expose<Connection>(m_c, event));
          return true;

        case XCB_GRAPHICS_EXPOSURE:
          handler(xpp::x::event::graphics_exposure<Connection>(m_c, event));
          return true;

        case XCB_NO_EXPOSURE:
          handler(xpp::x::event::no_exposure<Connection>(m_c, event));
          return true;

        case XCB_VISIBILITY_NOTIFY:
          handler(xpp::x::event::visibility_notify<Connection>(m_c, event));
          return true;

        case XCB_CREATE_NOTIFY:
          handler(xpp::x::event::create_notify<Connection>(m_c, event));
          return true;

        case XCB_DESTROY_NOTIFY:
          handler(xpp::x::event::destroy_notify<Connection>(m_c, event));
          return true;

        case XCB_UNMAP_NOTIFY:
          handler(xpp::x::event::unmap_notify<Connection>(m_c, event));
          return true;

        case XCB_MAP_NOTIFY:
          handler(xpp::x::event::map_notify<Connection>(m_c, event));
          return true;

        case XCB_MAP_REQUEST:
          handler(xpp::x::event::map_request<Connection>(m_c, event));
          return true;

        case XCB_REPARENT_NOTIFY:
          handler(xpp::x::event::reparent_notify<Connection>(m_c, event));
          return true;

        case XCB_CONFIGURE_NOTIFY:
          handler(xpp::x::event::configure_notify<Connection>(m_c, event));
          return true;

        case XCB_CONFIGURE_REQUEST:
          handler(xpp::x::event::configure_request<Connection>(m_c, event));
          return true;

        case XCB_GRAVITY_NOTIFY:
          handler(xpp::x::event::gravity_notify<Connection>(m_c, event));
          return true;

        case XCB_RESIZE_REQUEST:
          handler(xpp::x::event::resize_request<Connection>(m_c, event));
          return true;

        case XCB_CIRCULATE_NOTIFY:
          handler(xpp::x::event::circulate_notify<Connection>(m_c, event));
          return true;

        case XCB_CIRCULATE_REQUEST:
          handler(xpp::x::event::circulate_request<Connection>(m_c, event));
          return true;

        case XCB_PROPERTY_NOTIFY:
          handler(xpp::x::event::property_notify<Connection>(m_c, event));
          return true;

        case XCB_SELECTION_CLEAR:
          handler(xpp::x::event::selection_clear<Connection>(m_c, event));
          return true;

        case XCB_SELECTION_REQUEST:
          handler(xpp::x::event::selection_request<Connection>(m_c, event));
          return true;

        case XCB_SELECTION_NOTIFY:
          handler(xpp::x::event::selection_notify<Connection>(m_c, event));
          return true;

        case XCB_COLORMAP_NOTIFY:
          handler(xpp::x::event::colormap_notify<Connection>(m_c, event));
          return true;

        case XCB_CLIENT_MESSAGE:
          handler(xpp::x::event::client_message<Connection>(m_c, event));
          return true;

        case XCB_MAPPING_NOTIFY:
          handler(xpp::x::event::mapping_notify<Connection>(m_c, event));
          return true;

        case XCB_GE_GENERIC:
          handler(xpp::x::event::ge_generic<Connection>(m_c, event));
          return true;

      };

      return false;
    }

  protected:
    Connection m_c;
};

}

namespace error {

class dispatcher {
  public:
    typedef xpp::x::extension extension;


    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code) {

        case XCB_REQUEST: // 1
          throw xpp::x::error::request(error);

        case XCB_VALUE: // 2
          throw xpp::x::error::value(error);

        case XCB_WINDOW: // 3
          throw xpp::x::error::window(error);

        case XCB_PIXMAP: // 4
          throw xpp::x::error::pixmap(error);

        case XCB_ATOM: // 5
          throw xpp::x::error::atom(error);

        case XCB_CURSOR: // 6
          throw xpp::x::error::cursor(error);

        case XCB_FONT: // 7
          throw xpp::x::error::font(error);

        case XCB_MATCH: // 8
          throw xpp::x::error::match(error);

        case XCB_DRAWABLE: // 9
          throw xpp::x::error::drawable(error);

        case XCB_ACCESS: // 10
          throw xpp::x::error::access(error);

        case XCB_ALLOC: // 11
          throw xpp::x::error::alloc(error);

        case XCB_COLORMAP: // 12
          throw xpp::x::error::colormap(error);

        case XCB_G_CONTEXT: // 13
          throw xpp::x::error::g_context(error);

        case XCB_ID_CHOICE: // 14
          throw xpp::x::error::id_choice(error);

        case XCB_NAME: // 15
          throw xpp::x::error::name(error);

        case XCB_LENGTH: // 16
          throw xpp::x::error::length(error);

        case XCB_IMPLEMENTATION: // 17
          throw xpp::x::error::implementation(error);

      };
    }

};

}


};

#endif // XPP_X_HPP
