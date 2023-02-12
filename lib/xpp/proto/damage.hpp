#ifndef XPP_DAMAGE_HPP
#define XPP_DAMAGE_HPP

#include <string>
#include <vector>

#include <xcb/damage.h>

#include "xpp/generic.hpp"

namespace xpp::damage {

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
  : public xpp::generic::extension<extension, &xcb_damage_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_damage_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::damage::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::damage::event::dispatcher<Connection>;
    using error_dispatcher = xpp::damage::error::dispatcher;
};

namespace event {
template<typename Connection>
class notify : public xpp::generic::event<xcb_damage_notify_event_t> {
  public:
    typedef xpp::damage::extension extension;

    typedef xpp::generic::event<xcb_damage_notify_event_t> base;

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
      return XCB_DAMAGE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::damage::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_DAMAGE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
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

    template<typename ReturnType = xcb_damage_damage_t, typename ... Parameter>
    ReturnType damage(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->damage),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->damage,
                    std::forward<Parameter>(parameter) ...);
    }

  protected:
    Connection m_c;
    const uint8_t m_first_event;
};



}



namespace error {
class bad_damage : public xpp::generic::error<bad_damage, xcb_damage_bad_damage_error_t> {
  public:
    using xpp::generic::error<bad_damage, xcb_damage_bad_damage_error_t>::error;

    virtual ~bad_damage() = default;

    static uint8_t opcode() {
      return XCB_DAMAGE_BAD_DAMAGE;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::damage::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_DAMAGE_BAD_DAMAGE");
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
                               xpp::generic::signature<decltype(xcb_damage_query_version_reply), xcb_damage_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_damage_query_version_reply), xcb_damage_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_damage_query_version), xcb_damage_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_damage_query_version_unchecked), xcb_damage_query_version_unchecked>>;
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
void create_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::damage::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_damage_create_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create(Parameter && ... parameter) {
  xcb_damage_create(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void destroy_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::damage::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_damage_destroy_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void destroy(Parameter && ... parameter) {
  xcb_damage_destroy(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void subtract_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::damage::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_damage_subtract_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void subtract(Parameter && ... parameter) {
  xcb_damage_subtract(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void add_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::damage::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_damage_add_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void add(Parameter && ... parameter) {
  xcb_damage_add(std::forward<Parameter>(parameter) ...);
}





























template<typename Derived, typename Connection>
class damage
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_damage_damage_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~damage() = default;

    template<typename ... Parameter>
    void create_checked(Parameter && ... parameter) const {
      xpp::damage::create_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create(Parameter && ... parameter) const {
      xpp::damage::create(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::damage::destroy_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::damage::destroy(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void subtract_checked(Parameter && ... parameter) const {
      xpp::damage::subtract_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void subtract(Parameter && ... parameter) const {
      xpp::damage::subtract(connection(),
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
    typedef xpp::damage::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    damage() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::damage::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::damage::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_checked(Parameter && ... parameter) const {
      xpp::damage::create_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create(Parameter && ... parameter) const {
      xpp::damage::create(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void destroy_checked(Parameter && ... parameter) const {
      xpp::damage::destroy_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void destroy(Parameter && ... parameter) const {
      xpp::damage::destroy(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void subtract_checked(Parameter && ... parameter) const {
      xpp::damage::subtract_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void subtract(Parameter && ... parameter) const {
      xpp::damage::subtract(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_checked(Parameter && ... parameter) const {
      xpp::damage::add_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add(Parameter && ... parameter) const {
      xpp::damage::add(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::damage::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::damage::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_DAMAGE_NOTIFY:
          handler(xpp::damage::event::notify<Connection>(m_c, m_first_event, event));
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
    typedef xpp::damage::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::damage::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_DAMAGE_BAD_DAMAGE: // 0
          throw xpp::damage::error::bad_damage(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_DAMAGE_HPP
