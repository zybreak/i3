#ifndef XPP_SHAPE_HPP
#define XPP_SHAPE_HPP

#include <string>
#include <vector>

#include <xcb/shape.h>

#include "xpp/generic.hpp"

namespace xpp::shape {

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
  : public xpp::generic::extension<extension, &xcb_shape_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_shape_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::shape::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::shape::event::dispatcher<Connection>;
    using error_dispatcher = xpp::shape::error::dispatcher;
};

namespace event {
template<typename Connection>
class notify : public xpp::generic::event<xcb_shape_notify_event_t> {
  public:
    typedef xpp::shape::extension extension;

    typedef xpp::generic::event<xcb_shape_notify_event_t> base;

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
      return XCB_SHAPE_NOTIFY;
    }
    
    static uint8_t opcode(uint8_t first_event) {
      return first_event + opcode();
    }
    
    static uint8_t opcode(const xpp::shape::extension & extension) {
      return opcode(extension->first_event);
    }

    static std::string description() {
      return std::string("XCB_SHAPE_NOTIFY");
    }

    uint8_t first_event() {
      return m_first_event;
    }

    template<typename ReturnType = xcb_window_t, typename ... Parameter>
    ReturnType affected_window(Parameter && ... parameter) const {
      using make = xpp::generic::factory::make<Connection,
                                               decltype((*this)->affected_window),
                                               ReturnType,
                                               Parameter ...>;
      return make()(this->m_c,
                    (*this)->affected_window,
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
                               xpp::generic::signature<decltype(xcb_shape_query_version_reply), xcb_shape_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shape_query_version_reply), xcb_shape_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shape_query_version), xcb_shape_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shape_query_version_unchecked), xcb_shape_query_version_unchecked>>;
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
void rectangles_checked(Connection && c, xcb_shape_op_t operation, xcb_shape_kind_t destination_kind, uint8_t ordering, xcb_window_t destination_window, int16_t x_offset, int16_t y_offset, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xpp::generic::check<Connection, xpp::shape::error::dispatcher>(std::forward<Connection>(c), xcb_shape_rectangles_checked(std::forward<Connection>(c), operation, destination_kind, ordering, destination_window, x_offset, y_offset, rectangles_len, rectangles));
}

template<typename Connection>
void rectangles(Connection && c, xcb_shape_op_t operation, xcb_shape_kind_t destination_kind, uint8_t ordering, xcb_window_t destination_window, int16_t x_offset, int16_t y_offset, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xcb_shape_rectangles(std::forward<Connection>(c), operation, destination_kind, ordering, destination_window, x_offset, y_offset, rectangles_len, rectangles);
}

template<typename Connection, typename Rectangles_Iterator>
void rectangles_checked(Connection && c, xcb_shape_op_t operation, xcb_shape_kind_t destination_kind, uint8_t ordering, xcb_window_t destination_window, int16_t x_offset, int16_t y_offset, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xpp::generic::check<Connection, xpp::shape::error::dispatcher>(std::forward<Connection>(c), xcb_shape_rectangles_checked(std::forward<Connection>(c), operation, destination_kind, ordering, destination_window, x_offset, y_offset, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data())));
}

template<typename Connection, typename Rectangles_Iterator>
void rectangles(Connection && c, xcb_shape_op_t operation, xcb_shape_kind_t destination_kind, uint8_t ordering, xcb_window_t destination_window, int16_t x_offset, int16_t y_offset, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xcb_shape_rectangles(std::forward<Connection>(c), operation, destination_kind, ordering, destination_window, x_offset, y_offset, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data()));
}

template<typename Connection, typename ... Parameter>
void mask_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shape::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shape_mask_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void mask(Parameter && ... parameter) {
  xcb_shape_mask(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void combine_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shape::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shape_combine_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void combine(Parameter && ... parameter) {
  xcb_shape_combine(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void offset_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shape::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shape_offset_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void offset(Parameter && ... parameter) {
  xcb_shape_offset(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_extents : public xpp::generic::reply<query_extents<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_shape_query_extents_reply), xcb_shape_query_extents_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_extents<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shape_query_extents_reply), xcb_shape_query_extents_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_extents(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_extents = detail::query_extents<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shape_query_extents), xcb_shape_query_extents>>;
}

namespace unchecked {
template<typename Connection>
using query_extents = detail::query_extents<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shape_query_extents_unchecked), xcb_shape_query_extents_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_extents<Connection> query_extents(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_extents<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_extents<Connection> query_extents_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_extents<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void select_input_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::shape::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_shape_select_input_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void select_input(Parameter && ... parameter) {
  xcb_shape_select_input(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class input_selected : public xpp::generic::reply<input_selected<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_shape_input_selected_reply), xcb_shape_input_selected_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<input_selected<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shape_input_selected_reply), xcb_shape_input_selected_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    input_selected(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using input_selected = detail::input_selected<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shape_input_selected), xcb_shape_input_selected>>;
}

namespace unchecked {
template<typename Connection>
using input_selected = detail::input_selected<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shape_input_selected_unchecked), xcb_shape_input_selected_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::input_selected<Connection> input_selected(Connection && c, Parameter && ... parameter) {
  return reply::checked::input_selected<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::input_selected<Connection> input_selected_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::input_selected<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class get_rectangles : public xpp::generic::reply<get_rectangles<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_shape_get_rectangles_reply), xcb_shape_get_rectangles_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<get_rectangles<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_shape_get_rectangles_reply), xcb_shape_get_rectangles_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    get_rectangles(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_shape_get_rectangles_reply_t, xpp::generic::iterator<Connection,
                                              xcb_rectangle_t,
                                              xpp::generic::signature<decltype(xcb_shape_get_rectangles_rectangles), xcb_shape_get_rectangles_rectangles>,
                                              xpp::generic::signature<decltype(xcb_shape_get_rectangles_rectangles_length), xcb_shape_get_rectangles_rectangles_length>>>
    rectangles() {
      return xpp::generic::list<Connection,
                                xcb_shape_get_rectangles_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_rectangle_t,
                                                       xpp::generic::signature<decltype(xcb_shape_get_rectangles_rectangles), xcb_shape_get_rectangles_rectangles>,
                                                       xpp::generic::signature<decltype(xcb_shape_get_rectangles_rectangles_length), xcb_shape_get_rectangles_rectangles_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using get_rectangles = detail::get_rectangles<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_shape_get_rectangles), xcb_shape_get_rectangles>>;
}

namespace unchecked {
template<typename Connection>
using get_rectangles = detail::get_rectangles<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_shape_get_rectangles_unchecked), xcb_shape_get_rectangles_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::get_rectangles<Connection> get_rectangles(Connection && c, Parameter && ... parameter) {
  return reply::checked::get_rectangles<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::get_rectangles<Connection> get_rectangles_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::get_rectangles<Connection>(
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
    typedef xpp::shape::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    shape() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::shape::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::shape::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void rectangles_checked(Parameter && ... parameter) const {
      xpp::shape::rectangles_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void rectangles(Parameter && ... parameter) const {
      xpp::shape::rectangles(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void mask_checked(Parameter && ... parameter) const {
      xpp::shape::mask_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void mask(Parameter && ... parameter) const {
      xpp::shape::mask(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void combine_checked(Parameter && ... parameter) const {
      xpp::shape::combine_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void combine(Parameter && ... parameter) const {
      xpp::shape::combine(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void offset_checked(Parameter && ... parameter) const {
      xpp::shape::offset_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void offset(Parameter && ... parameter) const {
      xpp::shape::offset(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_extents(Parameter && ... parameter) const -> reply::checked::query_extents<Connection> {
      return xpp::shape::query_extents(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_extents_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_extents<Connection> {
      return xpp::shape::query_extents_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void select_input_checked(Parameter && ... parameter) const {
      xpp::shape::select_input_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void select_input(Parameter && ... parameter) const {
      xpp::shape::select_input(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto input_selected(Parameter && ... parameter) const -> reply::checked::input_selected<Connection> {
      return xpp::shape::input_selected(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto input_selected_unchecked(Parameter && ... parameter) const -> reply::unchecked::input_selected<Connection> {
      return xpp::shape::input_selected_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto get_rectangles(Parameter && ... parameter) const -> reply::checked::get_rectangles<Connection> {
      return xpp::shape::get_rectangles(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto get_rectangles_unchecked(Parameter && ... parameter) const -> reply::unchecked::get_rectangles<Connection> {
      return xpp::shape::get_rectangles_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::shape::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::shape::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

    template<typename Handler>
    bool operator()(Handler handler, const std::shared_ptr<xcb_generic_event_t> & event) const {
      switch ((event->response_type & ~0x80) - m_first_event) {

        case XCB_SHAPE_NOTIFY:
          handler(xpp::shape::event::notify<Connection>(m_c, m_first_event, event));
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
    typedef xpp::shape::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::shape::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> &) const {
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_SHAPE_HPP
