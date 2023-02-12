#ifndef XPP_RENDER_HPP
#define XPP_RENDER_HPP

#include <string>
#include <vector>

#include <xcb/render.h>

#include "xpp/generic.hpp"

namespace xpp::render {

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
  : public xpp::generic::extension<extension, &xcb_render_id>
 {
  public:
    using base = xpp::generic::extension<extension, &xcb_render_id>;
    using base::base;
    template<typename Derived, typename Connection>
    using interface = xpp::render::interface<Derived, Connection>;
    template<typename Connection>
    using event_dispatcher = xpp::render::event::dispatcher<Connection>;
    using error_dispatcher = xpp::render::error::dispatcher;
};


namespace error {
class pict_format : public xpp::generic::error<pict_format, xcb_render_pict_format_error_t> {
  public:
    using xpp::generic::error<pict_format, xcb_render_pict_format_error_t>::error;

    virtual ~pict_format() = default;

    static uint8_t opcode() {
      return XCB_RENDER_PICT_FORMAT;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::render::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RENDER_PICT_FORMAT");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class picture : public xpp::generic::error<picture, xcb_render_picture_error_t> {
  public:
    using xpp::generic::error<picture, xcb_render_picture_error_t>::error;

    virtual ~picture() = default;

    static uint8_t opcode() {
      return XCB_RENDER_PICTURE;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::render::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RENDER_PICTURE");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class pict_op : public xpp::generic::error<pict_op, xcb_render_pict_op_error_t> {
  public:
    using xpp::generic::error<pict_op, xcb_render_pict_op_error_t>::error;

    virtual ~pict_op() = default;

    static uint8_t opcode() {
      return XCB_RENDER_PICT_OP;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::render::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RENDER_PICT_OP");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class glyph_set : public xpp::generic::error<glyph_set, xcb_render_glyph_set_error_t> {
  public:
    using xpp::generic::error<glyph_set, xcb_render_glyph_set_error_t>::error;

    virtual ~glyph_set() = default;

    static uint8_t opcode() {
      return XCB_RENDER_GLYPH_SET;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::render::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RENDER_GLYPH_SET");
    }

  protected:
    uint8_t m_first_error;
};
} 


namespace error {
class glyph : public xpp::generic::error<glyph, xcb_render_glyph_error_t> {
  public:
    using xpp::generic::error<glyph, xcb_render_glyph_error_t>::error;

    virtual ~glyph() = default;

    static uint8_t opcode() {
      return XCB_RENDER_GLYPH;
    }
    
    static uint8_t opcode(uint8_t first_error) {
      return first_error + opcode();
    }
    
    static uint8_t opcode(const xpp::render::extension & extension) {
      return opcode(extension->first_error);
    }

    static std::string description() {
      return std::string("XCB_RENDER_GLYPH");
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
                               xpp::generic::signature<decltype(xcb_render_query_version_reply), xcb_render_query_version_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_version<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_render_query_version_reply), xcb_render_query_version_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_version(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}

};

}

namespace checked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_render_query_version), xcb_render_query_version>>;
}

namespace unchecked {
template<typename Connection>
using query_version = detail::query_version<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_render_query_version_unchecked), xcb_render_query_version_unchecked>>;
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
class query_pict_formats : public xpp::generic::reply<query_pict_formats<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_render_query_pict_formats_reply), xcb_render_query_pict_formats_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_pict_formats<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_render_query_pict_formats_reply), xcb_render_query_pict_formats_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_pict_formats(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_render_query_pict_formats_reply_t, xpp::generic::iterator<Connection,
                                              xcb_render_pictforminfo_t,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_formats_formats), xcb_render_query_pict_formats_formats>,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_formats_formats_length), xcb_render_query_pict_formats_formats_length>>>
    formats() {
      return xpp::generic::list<Connection,
                                xcb_render_query_pict_formats_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_render_pictforminfo_t,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_formats_formats), xcb_render_query_pict_formats_formats>,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_formats_formats_length), xcb_render_query_pict_formats_formats_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_render_query_pict_formats_reply_t, xpp::generic::iterator<Connection,
                                              xcb_render_pictscreen_t,
                                              xpp::generic::signature<decltype(xcb_render_pictscreen_next), xcb_render_pictscreen_next>,
                                              xpp::generic::signature<decltype(xcb_render_pictscreen_sizeof), xcb_render_pictscreen_sizeof>,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_formats_screens_iterator), xcb_render_query_pict_formats_screens_iterator>>>
    screens() {
      return xpp::generic::list<Connection,
                                xcb_render_query_pict_formats_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_render_pictscreen_t,
                                                       xpp::generic::signature<decltype(xcb_render_pictscreen_next), xcb_render_pictscreen_next>,
                                                       xpp::generic::signature<decltype(xcb_render_pictscreen_sizeof), xcb_render_pictscreen_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_formats_screens_iterator), xcb_render_query_pict_formats_screens_iterator>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_render_query_pict_formats_reply_t, xpp::generic::iterator<Connection,
                                              uint32_t,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_formats_subpixels), xcb_render_query_pict_formats_subpixels>,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_formats_subpixels_length), xcb_render_query_pict_formats_subpixels_length>>>
    subpixels() {
      return xpp::generic::list<Connection,
                                xcb_render_query_pict_formats_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint32_t,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_formats_subpixels), xcb_render_query_pict_formats_subpixels>,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_formats_subpixels_length), xcb_render_query_pict_formats_subpixels_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_pict_formats = detail::query_pict_formats<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_render_query_pict_formats), xcb_render_query_pict_formats>>;
}

namespace unchecked {
template<typename Connection>
using query_pict_formats = detail::query_pict_formats<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_render_query_pict_formats_unchecked), xcb_render_query_pict_formats_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_pict_formats<Connection> query_pict_formats(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_pict_formats<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_pict_formats<Connection> query_pict_formats_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_pict_formats<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_pict_index_values : public xpp::generic::reply<query_pict_index_values<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_render_query_pict_index_values_reply), xcb_render_query_pict_index_values_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_pict_index_values<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_render_query_pict_index_values_reply), xcb_render_query_pict_index_values_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_pict_index_values(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_render_query_pict_index_values_reply_t, xpp::generic::iterator<Connection,
                                              xcb_render_indexvalue_t,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_index_values_values), xcb_render_query_pict_index_values_values>,
                                              xpp::generic::signature<decltype(xcb_render_query_pict_index_values_values_length), xcb_render_query_pict_index_values_values_length>>>
    values() {
      return xpp::generic::list<Connection,
                                xcb_render_query_pict_index_values_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_render_indexvalue_t,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_index_values_values), xcb_render_query_pict_index_values_values>,
                                                       xpp::generic::signature<decltype(xcb_render_query_pict_index_values_values_length), xcb_render_query_pict_index_values_values_length>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_pict_index_values = detail::query_pict_index_values<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_render_query_pict_index_values), xcb_render_query_pict_index_values>>;
}

namespace unchecked {
template<typename Connection>
using query_pict_index_values = detail::query_pict_index_values<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_render_query_pict_index_values_unchecked), xcb_render_query_pict_index_values_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_pict_index_values<Connection> query_pict_index_values(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_pict_index_values<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_pict_index_values<Connection> query_pict_index_values_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_pict_index_values<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_picture_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_picture_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_picture(Parameter && ... parameter) {
  xcb_render_create_picture(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void change_picture_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_change_picture_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void change_picture(Parameter && ... parameter) {
  xcb_render_change_picture(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_picture_clip_rectangles_checked(Connection && c, xcb_render_picture_t picture, int16_t clip_x_origin, int16_t clip_y_origin, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_set_picture_clip_rectangles_checked(std::forward<Connection>(c), picture, clip_x_origin, clip_y_origin, rectangles_len, rectangles));
}

template<typename Connection>
void set_picture_clip_rectangles(Connection && c, xcb_render_picture_t picture, int16_t clip_x_origin, int16_t clip_y_origin, uint32_t rectangles_len, const xcb_rectangle_t * rectangles) {
  xcb_render_set_picture_clip_rectangles(std::forward<Connection>(c), picture, clip_x_origin, clip_y_origin, rectangles_len, rectangles);
}

template<typename Connection, typename Rectangles_Iterator>
void set_picture_clip_rectangles_checked(Connection && c, xcb_render_picture_t picture, int16_t clip_x_origin, int16_t clip_y_origin, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_set_picture_clip_rectangles_checked(std::forward<Connection>(c), picture, clip_x_origin, clip_y_origin, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data())));
}

template<typename Connection, typename Rectangles_Iterator>
void set_picture_clip_rectangles(Connection && c, xcb_render_picture_t picture, int16_t clip_x_origin, int16_t clip_y_origin, Rectangles_Iterator rectangles_begin, Rectangles_Iterator rectangles_end) {
      typedef typename value_type<Rectangles_Iterator, ! std::is_pointer<Rectangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rectangles =
        { value_iterator<Rectangles_Iterator>(rectangles_begin), value_iterator<Rectangles_Iterator>(rectangles_end) };
      
  xcb_render_set_picture_clip_rectangles(std::forward<Connection>(c), picture, clip_x_origin, clip_y_origin, static_cast<uint32_t>(rectangles.size()), const_cast<const vector_type *>(rectangles.data()));
}

template<typename Connection, typename ... Parameter>
void free_picture_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_free_picture_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_picture(Parameter && ... parameter) {
  xcb_render_free_picture(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void composite_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_composite_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void composite(Parameter && ... parameter) {
  xcb_render_composite(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void trapezoids_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t traps_len, const xcb_render_trapezoid_t * traps) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_trapezoids_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, traps_len, traps));
}

template<typename Connection>
void trapezoids(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t traps_len, const xcb_render_trapezoid_t * traps) {
  xcb_render_trapezoids(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, traps_len, traps);
}

template<typename Connection, typename Traps_Iterator>
void trapezoids_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Traps_Iterator traps_begin, Traps_Iterator traps_end) {
      typedef typename value_type<Traps_Iterator, ! std::is_pointer<Traps_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> traps =
        { value_iterator<Traps_Iterator>(traps_begin), value_iterator<Traps_Iterator>(traps_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_trapezoids_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(traps.size()), const_cast<const vector_type *>(traps.data())));
}

template<typename Connection, typename Traps_Iterator>
void trapezoids(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Traps_Iterator traps_begin, Traps_Iterator traps_end) {
      typedef typename value_type<Traps_Iterator, ! std::is_pointer<Traps_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> traps =
        { value_iterator<Traps_Iterator>(traps_begin), value_iterator<Traps_Iterator>(traps_end) };
      
  xcb_render_trapezoids(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(traps.size()), const_cast<const vector_type *>(traps.data()));
}

template<typename Connection>
void triangles_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t triangles_len, const xcb_render_triangle_t * triangles) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_triangles_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, triangles_len, triangles));
}

template<typename Connection>
void triangles(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t triangles_len, const xcb_render_triangle_t * triangles) {
  xcb_render_triangles(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, triangles_len, triangles);
}

template<typename Connection, typename Triangles_Iterator>
void triangles_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Triangles_Iterator triangles_begin, Triangles_Iterator triangles_end) {
      typedef typename value_type<Triangles_Iterator, ! std::is_pointer<Triangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> triangles =
        { value_iterator<Triangles_Iterator>(triangles_begin), value_iterator<Triangles_Iterator>(triangles_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_triangles_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(triangles.size()), const_cast<const vector_type *>(triangles.data())));
}

template<typename Connection, typename Triangles_Iterator>
void triangles(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Triangles_Iterator triangles_begin, Triangles_Iterator triangles_end) {
      typedef typename value_type<Triangles_Iterator, ! std::is_pointer<Triangles_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> triangles =
        { value_iterator<Triangles_Iterator>(triangles_begin), value_iterator<Triangles_Iterator>(triangles_end) };
      
  xcb_render_triangles(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(triangles.size()), const_cast<const vector_type *>(triangles.data()));
}

template<typename Connection>
void tri_strip_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t points_len, const xcb_render_pointfix_t * points) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_tri_strip_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, points_len, points));
}

template<typename Connection>
void tri_strip(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t points_len, const xcb_render_pointfix_t * points) {
  xcb_render_tri_strip(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, points_len, points);
}

template<typename Connection, typename Points_Iterator>
void tri_strip_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_tri_strip_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data())));
}

template<typename Connection, typename Points_Iterator>
void tri_strip(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xcb_render_tri_strip(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data()));
}

template<typename Connection>
void tri_fan_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t points_len, const xcb_render_pointfix_t * points) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_tri_fan_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, points_len, points));
}

template<typename Connection>
void tri_fan(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, uint32_t points_len, const xcb_render_pointfix_t * points) {
  xcb_render_tri_fan(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, points_len, points);
}

template<typename Connection, typename Points_Iterator>
void tri_fan_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_tri_fan_checked(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data())));
}

template<typename Connection, typename Points_Iterator>
void tri_fan(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, int16_t src_x, int16_t src_y, Points_Iterator points_begin, Points_Iterator points_end) {
      typedef typename value_type<Points_Iterator, ! std::is_pointer<Points_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> points =
        { value_iterator<Points_Iterator>(points_begin), value_iterator<Points_Iterator>(points_end) };
      
  xcb_render_tri_fan(std::forward<Connection>(c), op, src, dst, mask_format, src_x, src_y, static_cast<uint32_t>(points.size()), const_cast<const vector_type *>(points.data()));
}

template<typename Connection, typename ... Parameter>
void create_glyph_set_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_glyph_set_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_glyph_set(Parameter && ... parameter) {
  xcb_render_create_glyph_set(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void reference_glyph_set_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_reference_glyph_set_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void reference_glyph_set(Parameter && ... parameter) {
  xcb_render_reference_glyph_set(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void free_glyph_set_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_free_glyph_set_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void free_glyph_set(Parameter && ... parameter) {
  xcb_render_free_glyph_set(std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void add_glyphs_checked(Connection && c, xcb_render_glyphset_t glyphset, uint32_t glyphs_len, const uint32_t * glyphids, const xcb_render_glyphinfo_t * glyphs, uint32_t data_len, const uint8_t * data) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_add_glyphs_checked(std::forward<Connection>(c), glyphset, glyphs_len, glyphids, glyphs, data_len, data));
}

template<typename Connection>
void add_glyphs(Connection && c, xcb_render_glyphset_t glyphset, uint32_t glyphs_len, const uint32_t * glyphids, const xcb_render_glyphinfo_t * glyphs, uint32_t data_len, const uint8_t * data) {
  xcb_render_add_glyphs(std::forward<Connection>(c), glyphset, glyphs_len, glyphids, glyphs, data_len, data);
}

template<typename Connection, typename Data_Iterator>
void add_glyphs_checked(Connection && c, xcb_render_glyphset_t glyphset, uint32_t glyphs_len, const uint32_t * glyphids, const xcb_render_glyphinfo_t * glyphs, Data_Iterator data_begin, Data_Iterator data_end) {
      typedef typename value_type<Data_Iterator, ! std::is_pointer<Data_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> data =
        { value_iterator<Data_Iterator>(data_begin), value_iterator<Data_Iterator>(data_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_add_glyphs_checked(std::forward<Connection>(c), glyphset, glyphs_len, glyphids, glyphs, static_cast<uint32_t>(data.size()), const_cast<const vector_type *>(data.data())));
}

template<typename Connection, typename Data_Iterator>
void add_glyphs(Connection && c, xcb_render_glyphset_t glyphset, uint32_t glyphs_len, const uint32_t * glyphids, const xcb_render_glyphinfo_t * glyphs, Data_Iterator data_begin, Data_Iterator data_end) {
      typedef typename value_type<Data_Iterator, ! std::is_pointer<Data_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> data =
        { value_iterator<Data_Iterator>(data_begin), value_iterator<Data_Iterator>(data_end) };
      
  xcb_render_add_glyphs(std::forward<Connection>(c), glyphset, glyphs_len, glyphids, glyphs, static_cast<uint32_t>(data.size()), const_cast<const vector_type *>(data.data()));
}

template<typename Connection>
void free_glyphs_checked(Connection && c, xcb_render_glyphset_t glyphset, uint32_t glyphs_len, const xcb_render_glyph_t * glyphs) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_free_glyphs_checked(std::forward<Connection>(c), glyphset, glyphs_len, glyphs));
}

template<typename Connection>
void free_glyphs(Connection && c, xcb_render_glyphset_t glyphset, uint32_t glyphs_len, const xcb_render_glyph_t * glyphs) {
  xcb_render_free_glyphs(std::forward<Connection>(c), glyphset, glyphs_len, glyphs);
}

template<typename Connection, typename Glyphs_Iterator>
void free_glyphs_checked(Connection && c, xcb_render_glyphset_t glyphset, Glyphs_Iterator glyphs_begin, Glyphs_Iterator glyphs_end) {
      typedef typename value_type<Glyphs_Iterator, ! std::is_pointer<Glyphs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphs =
        { value_iterator<Glyphs_Iterator>(glyphs_begin), value_iterator<Glyphs_Iterator>(glyphs_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_free_glyphs_checked(std::forward<Connection>(c), glyphset, static_cast<uint32_t>(glyphs.size()), const_cast<const vector_type *>(glyphs.data())));
}

template<typename Connection, typename Glyphs_Iterator>
void free_glyphs(Connection && c, xcb_render_glyphset_t glyphset, Glyphs_Iterator glyphs_begin, Glyphs_Iterator glyphs_end) {
      typedef typename value_type<Glyphs_Iterator, ! std::is_pointer<Glyphs_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphs =
        { value_iterator<Glyphs_Iterator>(glyphs_begin), value_iterator<Glyphs_Iterator>(glyphs_end) };
      
  xcb_render_free_glyphs(std::forward<Connection>(c), glyphset, static_cast<uint32_t>(glyphs.size()), const_cast<const vector_type *>(glyphs.data()));
}

template<typename Connection>
void composite_glyphs_8_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, uint32_t glyphcmds_len, const uint8_t * glyphcmds) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_composite_glyphs_8_checked(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, glyphcmds_len, glyphcmds));
}

template<typename Connection>
void composite_glyphs_8(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, uint32_t glyphcmds_len, const uint8_t * glyphcmds) {
  xcb_render_composite_glyphs_8(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, glyphcmds_len, glyphcmds);
}

template<typename Connection, typename Glyphcmds_Iterator>
void composite_glyphs_8_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, Glyphcmds_Iterator glyphcmds_begin, Glyphcmds_Iterator glyphcmds_end) {
      typedef typename value_type<Glyphcmds_Iterator, ! std::is_pointer<Glyphcmds_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphcmds =
        { value_iterator<Glyphcmds_Iterator>(glyphcmds_begin), value_iterator<Glyphcmds_Iterator>(glyphcmds_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_composite_glyphs_8_checked(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, static_cast<uint32_t>(glyphcmds.size()), const_cast<const vector_type *>(glyphcmds.data())));
}

template<typename Connection, typename Glyphcmds_Iterator>
void composite_glyphs_8(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, Glyphcmds_Iterator glyphcmds_begin, Glyphcmds_Iterator glyphcmds_end) {
      typedef typename value_type<Glyphcmds_Iterator, ! std::is_pointer<Glyphcmds_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphcmds =
        { value_iterator<Glyphcmds_Iterator>(glyphcmds_begin), value_iterator<Glyphcmds_Iterator>(glyphcmds_end) };
      
  xcb_render_composite_glyphs_8(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, static_cast<uint32_t>(glyphcmds.size()), const_cast<const vector_type *>(glyphcmds.data()));
}

template<typename Connection>
void composite_glyphs_16_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, uint32_t glyphcmds_len, const uint8_t * glyphcmds) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_composite_glyphs_16_checked(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, glyphcmds_len, glyphcmds));
}

template<typename Connection>
void composite_glyphs_16(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, uint32_t glyphcmds_len, const uint8_t * glyphcmds) {
  xcb_render_composite_glyphs_16(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, glyphcmds_len, glyphcmds);
}

template<typename Connection, typename Glyphcmds_Iterator>
void composite_glyphs_16_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, Glyphcmds_Iterator glyphcmds_begin, Glyphcmds_Iterator glyphcmds_end) {
      typedef typename value_type<Glyphcmds_Iterator, ! std::is_pointer<Glyphcmds_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphcmds =
        { value_iterator<Glyphcmds_Iterator>(glyphcmds_begin), value_iterator<Glyphcmds_Iterator>(glyphcmds_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_composite_glyphs_16_checked(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, static_cast<uint32_t>(glyphcmds.size()), const_cast<const vector_type *>(glyphcmds.data())));
}

template<typename Connection, typename Glyphcmds_Iterator>
void composite_glyphs_16(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, Glyphcmds_Iterator glyphcmds_begin, Glyphcmds_Iterator glyphcmds_end) {
      typedef typename value_type<Glyphcmds_Iterator, ! std::is_pointer<Glyphcmds_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphcmds =
        { value_iterator<Glyphcmds_Iterator>(glyphcmds_begin), value_iterator<Glyphcmds_Iterator>(glyphcmds_end) };
      
  xcb_render_composite_glyphs_16(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, static_cast<uint32_t>(glyphcmds.size()), const_cast<const vector_type *>(glyphcmds.data()));
}

template<typename Connection>
void composite_glyphs_32_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, uint32_t glyphcmds_len, const uint8_t * glyphcmds) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_composite_glyphs_32_checked(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, glyphcmds_len, glyphcmds));
}

template<typename Connection>
void composite_glyphs_32(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, uint32_t glyphcmds_len, const uint8_t * glyphcmds) {
  xcb_render_composite_glyphs_32(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, glyphcmds_len, glyphcmds);
}

template<typename Connection, typename Glyphcmds_Iterator>
void composite_glyphs_32_checked(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, Glyphcmds_Iterator glyphcmds_begin, Glyphcmds_Iterator glyphcmds_end) {
      typedef typename value_type<Glyphcmds_Iterator, ! std::is_pointer<Glyphcmds_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphcmds =
        { value_iterator<Glyphcmds_Iterator>(glyphcmds_begin), value_iterator<Glyphcmds_Iterator>(glyphcmds_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_composite_glyphs_32_checked(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, static_cast<uint32_t>(glyphcmds.size()), const_cast<const vector_type *>(glyphcmds.data())));
}

template<typename Connection, typename Glyphcmds_Iterator>
void composite_glyphs_32(Connection && c, uint8_t op, xcb_render_picture_t src, xcb_render_picture_t dst, xcb_render_pictformat_t mask_format, xcb_render_glyphset_t glyphset, int16_t src_x, int16_t src_y, Glyphcmds_Iterator glyphcmds_begin, Glyphcmds_Iterator glyphcmds_end) {
      typedef typename value_type<Glyphcmds_Iterator, ! std::is_pointer<Glyphcmds_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> glyphcmds =
        { value_iterator<Glyphcmds_Iterator>(glyphcmds_begin), value_iterator<Glyphcmds_Iterator>(glyphcmds_end) };
      
  xcb_render_composite_glyphs_32(std::forward<Connection>(c), op, src, dst, mask_format, glyphset, src_x, src_y, static_cast<uint32_t>(glyphcmds.size()), const_cast<const vector_type *>(glyphcmds.data()));
}

template<typename Connection>
void fill_rectangles_checked(Connection && c, uint8_t op, xcb_render_picture_t dst, xcb_render_color_t color, uint32_t rects_len, const xcb_rectangle_t * rects) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_fill_rectangles_checked(std::forward<Connection>(c), op, dst, color, rects_len, rects));
}

template<typename Connection>
void fill_rectangles(Connection && c, uint8_t op, xcb_render_picture_t dst, xcb_render_color_t color, uint32_t rects_len, const xcb_rectangle_t * rects) {
  xcb_render_fill_rectangles(std::forward<Connection>(c), op, dst, color, rects_len, rects);
}

template<typename Connection, typename Rects_Iterator>
void fill_rectangles_checked(Connection && c, uint8_t op, xcb_render_picture_t dst, xcb_render_color_t color, Rects_Iterator rects_begin, Rects_Iterator rects_end) {
      typedef typename value_type<Rects_Iterator, ! std::is_pointer<Rects_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rects =
        { value_iterator<Rects_Iterator>(rects_begin), value_iterator<Rects_Iterator>(rects_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_fill_rectangles_checked(std::forward<Connection>(c), op, dst, color, static_cast<uint32_t>(rects.size()), const_cast<const vector_type *>(rects.data())));
}

template<typename Connection, typename Rects_Iterator>
void fill_rectangles(Connection && c, uint8_t op, xcb_render_picture_t dst, xcb_render_color_t color, Rects_Iterator rects_begin, Rects_Iterator rects_end) {
      typedef typename value_type<Rects_Iterator, ! std::is_pointer<Rects_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> rects =
        { value_iterator<Rects_Iterator>(rects_begin), value_iterator<Rects_Iterator>(rects_end) };
      
  xcb_render_fill_rectangles(std::forward<Connection>(c), op, dst, color, static_cast<uint32_t>(rects.size()), const_cast<const vector_type *>(rects.data()));
}

template<typename Connection, typename ... Parameter>
void create_cursor_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_cursor_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_cursor(Parameter && ... parameter) {
  xcb_render_create_cursor(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void set_picture_transform_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_set_picture_transform_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void set_picture_transform(Parameter && ... parameter) {
  xcb_render_set_picture_transform(std::forward<Parameter>(parameter) ...);
}

namespace reply {

namespace detail {

template<typename Connection, typename Check, typename CookieFunction>
class query_filters : public xpp::generic::reply<query_filters<Connection, Check, CookieFunction>,
                               Connection,
                               Check,
                               xpp::generic::signature<decltype(xcb_render_query_filters_reply), xcb_render_query_filters_reply>,
                               CookieFunction>
{
  public:
    typedef xpp::generic::reply<query_filters<Connection, Check, CookieFunction>,
                                Connection,
                                Check,
                                xpp::generic::signature<decltype(xcb_render_query_filters_reply), xcb_render_query_filters_reply>,
                                CookieFunction>
                                  base;

    template<typename C, typename ... Parameter>
    query_filters(C && c, Parameter && ... parameter) : base(std::forward<C>(c), std::forward<Parameter>(parameter) ...)
    {}


    xpp::generic::list<Connection, xcb_render_query_filters_reply_t, xpp::generic::iterator<Connection,
                                              uint16_t,
                                              xpp::generic::signature<decltype(xcb_render_query_filters_aliases), xcb_render_query_filters_aliases>,
                                              xpp::generic::signature<decltype(xcb_render_query_filters_aliases_length), xcb_render_query_filters_aliases_length>>>
    aliases() {
      return xpp::generic::list<Connection,
                                xcb_render_query_filters_reply_t,
                                xpp::generic::iterator<Connection,
                                                       uint16_t,
                                                       xpp::generic::signature<decltype(xcb_render_query_filters_aliases), xcb_render_query_filters_aliases>,
                                                       xpp::generic::signature<decltype(xcb_render_query_filters_aliases_length), xcb_render_query_filters_aliases_length>>
                               >(this->m_c, this->get());
    }


    xpp::generic::list<Connection, xcb_render_query_filters_reply_t, xpp::generic::iterator<Connection,
                                              xcb_str_t,
                                              xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                              xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                              xpp::generic::signature<decltype(xcb_render_query_filters_filters_iterator), xcb_render_query_filters_filters_iterator>>>
    filters() {
      return xpp::generic::list<Connection,
                                xcb_render_query_filters_reply_t,
                                xpp::generic::iterator<Connection,
                                                       xcb_str_t,
                                                       xpp::generic::signature<decltype(xcb_str_next), xcb_str_next>,
                                                       xpp::generic::signature<decltype(xcb_str_sizeof), xcb_str_sizeof>,
                                                       xpp::generic::signature<decltype(xcb_render_query_filters_filters_iterator), xcb_render_query_filters_filters_iterator>>
                               >(this->m_c, this->get());
    }
};

}

namespace checked {
template<typename Connection>
using query_filters = detail::query_filters<Connection, xpp::generic::checked_tag, xpp::generic::signature<decltype(xcb_render_query_filters), xcb_render_query_filters>>;
}

namespace unchecked {
template<typename Connection>
using query_filters = detail::query_filters<Connection, xpp::generic::unchecked_tag, xpp::generic::signature<decltype(xcb_render_query_filters_unchecked), xcb_render_query_filters_unchecked>>;
}

}


template<typename Connection, typename ... Parameter>
reply::checked::query_filters<Connection> query_filters(Connection && c, Parameter && ... parameter) {
  return reply::checked::query_filters<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
reply::unchecked::query_filters<Connection> query_filters_unchecked(Connection && c, Parameter && ... parameter) {
  return reply::unchecked::query_filters<Connection>(
      std::forward<Connection>(c), std::forward<Parameter>(parameter) ...);
}

template<typename Connection>
void set_picture_filter_checked(Connection && c, xcb_render_picture_t picture, uint16_t filter_len, const char * filter, uint32_t values_len, const xcb_render_fixed_t * values) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_set_picture_filter_checked(std::forward<Connection>(c), picture, filter_len, filter, values_len, values));
}

template<typename Connection>
void set_picture_filter(Connection && c, xcb_render_picture_t picture, uint16_t filter_len, const char * filter, uint32_t values_len, const xcb_render_fixed_t * values) {
  xcb_render_set_picture_filter(std::forward<Connection>(c), picture, filter_len, filter, values_len, values);
}

template<typename Connection>
void set_picture_filter_checked(Connection && c, xcb_render_picture_t picture, const std::string & filter, uint32_t values_len, const xcb_render_fixed_t * values) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_set_picture_filter_checked(std::forward<Connection>(c), picture, static_cast<uint16_t>(filter.length()), filter.c_str(), values_len, values));
}

template<typename Connection>
void set_picture_filter(Connection && c, xcb_render_picture_t picture, const std::string & filter, uint32_t values_len, const xcb_render_fixed_t * values) {
  xcb_render_set_picture_filter(std::forward<Connection>(c), picture, static_cast<uint16_t>(filter.length()), filter.c_str(), values_len, values);
}

template<typename Connection>
void create_anim_cursor_checked(Connection && c, xcb_cursor_t cid, uint32_t cursors_len, const xcb_render_animcursorelt_t * cursors) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_create_anim_cursor_checked(std::forward<Connection>(c), cid, cursors_len, cursors));
}

template<typename Connection>
void create_anim_cursor(Connection && c, xcb_cursor_t cid, uint32_t cursors_len, const xcb_render_animcursorelt_t * cursors) {
  xcb_render_create_anim_cursor(std::forward<Connection>(c), cid, cursors_len, cursors);
}

template<typename Connection, typename Cursors_Iterator>
void create_anim_cursor_checked(Connection && c, xcb_cursor_t cid, Cursors_Iterator cursors_begin, Cursors_Iterator cursors_end) {
      typedef typename value_type<Cursors_Iterator, ! std::is_pointer<Cursors_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> cursors =
        { value_iterator<Cursors_Iterator>(cursors_begin), value_iterator<Cursors_Iterator>(cursors_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_create_anim_cursor_checked(std::forward<Connection>(c), cid, static_cast<uint32_t>(cursors.size()), const_cast<const vector_type *>(cursors.data())));
}

template<typename Connection, typename Cursors_Iterator>
void create_anim_cursor(Connection && c, xcb_cursor_t cid, Cursors_Iterator cursors_begin, Cursors_Iterator cursors_end) {
      typedef typename value_type<Cursors_Iterator, ! std::is_pointer<Cursors_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> cursors =
        { value_iterator<Cursors_Iterator>(cursors_begin), value_iterator<Cursors_Iterator>(cursors_end) };
      
  xcb_render_create_anim_cursor(std::forward<Connection>(c), cid, static_cast<uint32_t>(cursors.size()), const_cast<const vector_type *>(cursors.data()));
}

template<typename Connection>
void add_traps_checked(Connection && c, xcb_render_picture_t picture, int16_t x_off, int16_t y_off, uint32_t traps_len, const xcb_render_trap_t * traps) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_add_traps_checked(std::forward<Connection>(c), picture, x_off, y_off, traps_len, traps));
}

template<typename Connection>
void add_traps(Connection && c, xcb_render_picture_t picture, int16_t x_off, int16_t y_off, uint32_t traps_len, const xcb_render_trap_t * traps) {
  xcb_render_add_traps(std::forward<Connection>(c), picture, x_off, y_off, traps_len, traps);
}

template<typename Connection, typename Traps_Iterator>
void add_traps_checked(Connection && c, xcb_render_picture_t picture, int16_t x_off, int16_t y_off, Traps_Iterator traps_begin, Traps_Iterator traps_end) {
      typedef typename value_type<Traps_Iterator, ! std::is_pointer<Traps_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> traps =
        { value_iterator<Traps_Iterator>(traps_begin), value_iterator<Traps_Iterator>(traps_end) };
      
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(std::forward<Connection>(c), xcb_render_add_traps_checked(std::forward<Connection>(c), picture, x_off, y_off, static_cast<uint32_t>(traps.size()), const_cast<const vector_type *>(traps.data())));
}

template<typename Connection, typename Traps_Iterator>
void add_traps(Connection && c, xcb_render_picture_t picture, int16_t x_off, int16_t y_off, Traps_Iterator traps_begin, Traps_Iterator traps_end) {
      typedef typename value_type<Traps_Iterator, ! std::is_pointer<Traps_Iterator>::value>::type
                vector_type;
      std::vector<vector_type> traps =
        { value_iterator<Traps_Iterator>(traps_begin), value_iterator<Traps_Iterator>(traps_end) };
      
  xcb_render_add_traps(std::forward<Connection>(c), picture, x_off, y_off, static_cast<uint32_t>(traps.size()), const_cast<const vector_type *>(traps.data()));
}

template<typename Connection, typename ... Parameter>
void create_solid_fill_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_solid_fill_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_solid_fill(Parameter && ... parameter) {
  xcb_render_create_solid_fill(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_linear_gradient_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_linear_gradient_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_linear_gradient(Parameter && ... parameter) {
  xcb_render_create_linear_gradient(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_radial_gradient_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_radial_gradient_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_radial_gradient(Parameter && ... parameter) {
  xcb_render_create_radial_gradient(std::forward<Parameter>(parameter) ...);
}

template<typename Connection, typename ... Parameter>
void create_conical_gradient_checked(Connection && c, Parameter && ... parameter) {
  xpp::generic::check<Connection, xpp::render::error::dispatcher>(
      std::forward<Connection>(c),
      xcb_render_create_conical_gradient_checked(
          std::forward<Connection>(c),
          std::forward<Parameter>(parameter) ...));
}

template<typename ... Parameter>
void create_conical_gradient(Parameter && ... parameter) {
  xcb_render_create_conical_gradient(std::forward<Parameter>(parameter) ...);
}














template<typename Derived, typename Connection>
class picture
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_render_picture_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~picture() = default;

    template<typename ... Parameter>
    void create_checked(Parameter && ... parameter) const {
      xpp::render::create_picture_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create(Parameter && ... parameter) const {
      xpp::render::create_picture(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_checked(Parameter && ... parameter) const {
      xpp::render::change_picture_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change(Parameter && ... parameter) const {
      xpp::render::change_picture(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_clip_rectangles_checked(Parameter && ... parameter) const {
      xpp::render::set_picture_clip_rectangles_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_clip_rectangles(Parameter && ... parameter) const {
      xpp::render::set_picture_clip_rectangles(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_checked(Parameter && ... parameter) const {
      xpp::render::free_picture_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free(Parameter && ... parameter) const {
      xpp::render::free_picture(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_transform_checked(Parameter && ... parameter) const {
      xpp::render::set_picture_transform_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_transform(Parameter && ... parameter) const {
      xpp::render::set_picture_transform(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_filter_checked(Parameter && ... parameter) const {
      xpp::render::set_picture_filter_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_filter(Parameter && ... parameter) const {
      xpp::render::set_picture_filter(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_traps_checked(Parameter && ... parameter) const {
      xpp::render::add_traps_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add_traps(Parameter && ... parameter) const {
      xpp::render::add_traps(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_solid_fill_checked(Parameter && ... parameter) const {
      xpp::render::create_solid_fill_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_solid_fill(Parameter && ... parameter) const {
      xpp::render::create_solid_fill(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_linear_gradient_checked(Parameter && ... parameter) const {
      xpp::render::create_linear_gradient_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_linear_gradient(Parameter && ... parameter) const {
      xpp::render::create_linear_gradient(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_radial_gradient_checked(Parameter && ... parameter) const {
      xpp::render::create_radial_gradient_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_radial_gradient(Parameter && ... parameter) const {
      xpp::render::create_radial_gradient(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_conical_gradient_checked(Parameter && ... parameter) const {
      xpp::render::create_conical_gradient_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_conical_gradient(Parameter && ... parameter) const {
      xpp::render::create_conical_gradient(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }



};




template<typename Derived, typename Connection>
class pictformat
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_render_pictformat_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~pictformat() = default;

    template<typename ... Parameter>
    auto query_pict_index_values(Parameter && ... parameter) const -> reply::checked::query_pict_index_values<Connection> {
      return xpp::render::query_pict_index_values(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_pict_index_values_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_pict_index_values<Connection> {
      return xpp::render::query_pict_index_values_unchecked(
          connection(),
          resource(),
          std::forward<Parameter>(parameter) ...);
    }



};















template<typename Derived, typename Connection>
class glyphset
{
  protected:
    Connection connection() const {
      return static_cast<const Derived *>(this)->connection();
    }

    const xcb_render_glyphset_t & resource() const {
      return static_cast<const Derived *>(this)->resource();
    }

  public:
    virtual ~glyphset() = default;

    template<typename ... Parameter>
    void create_glyph_set_checked(Parameter && ... parameter) const {
      xpp::render::create_glyph_set_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_glyph_set(Parameter && ... parameter) const {
      xpp::render::create_glyph_set(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void reference_glyph_set_checked(Parameter && ... parameter) const {
      xpp::render::reference_glyph_set_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void reference_glyph_set(Parameter && ... parameter) const {
      xpp::render::reference_glyph_set(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_glyph_set_checked(Parameter && ... parameter) const {
      xpp::render::free_glyph_set_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_glyph_set(Parameter && ... parameter) const {
      xpp::render::free_glyph_set(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_glyphs_checked(Parameter && ... parameter) const {
      xpp::render::add_glyphs_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add_glyphs(Parameter && ... parameter) const {
      xpp::render::add_glyphs(connection(),
                  resource(),
                  std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_glyphs_checked(Parameter && ... parameter) const {
      xpp::render::free_glyphs_checked(connection(),
                          resource(),
                          std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_glyphs(Parameter && ... parameter) const {
      xpp::render::free_glyphs(connection(),
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
    typedef xpp::render::extension extension;


    virtual ~interface() = default;

    const interface<Derived, Connection> &
    render() {
      return *this;
    }

    template<typename ... Parameter>
    auto query_version(Parameter && ... parameter) const -> reply::checked::query_version<Connection> {
      return xpp::render::query_version(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_version_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_version<Connection> {
      return xpp::render::query_version_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_pict_formats(Parameter && ... parameter) const -> reply::checked::query_pict_formats<Connection> {
      return xpp::render::query_pict_formats(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_pict_formats_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_pict_formats<Connection> {
      return xpp::render::query_pict_formats_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_pict_index_values(Parameter && ... parameter) const -> reply::checked::query_pict_index_values<Connection> {
      return xpp::render::query_pict_index_values(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_pict_index_values_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_pict_index_values<Connection> {
      return xpp::render::query_pict_index_values_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_picture_checked(Parameter && ... parameter) const {
      xpp::render::create_picture_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_picture(Parameter && ... parameter) const {
      xpp::render::create_picture(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void change_picture_checked(Parameter && ... parameter) const {
      xpp::render::change_picture_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void change_picture(Parameter && ... parameter) const {
      xpp::render::change_picture(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_picture_clip_rectangles_checked(Parameter && ... parameter) const {
      xpp::render::set_picture_clip_rectangles_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_picture_clip_rectangles(Parameter && ... parameter) const {
      xpp::render::set_picture_clip_rectangles(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_picture_checked(Parameter && ... parameter) const {
      xpp::render::free_picture_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_picture(Parameter && ... parameter) const {
      xpp::render::free_picture(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void composite_checked(Parameter && ... parameter) const {
      xpp::render::composite_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void composite(Parameter && ... parameter) const {
      xpp::render::composite(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void trapezoids_checked(Parameter && ... parameter) const {
      xpp::render::trapezoids_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void trapezoids(Parameter && ... parameter) const {
      xpp::render::trapezoids(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void triangles_checked(Parameter && ... parameter) const {
      xpp::render::triangles_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void triangles(Parameter && ... parameter) const {
      xpp::render::triangles(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void tri_strip_checked(Parameter && ... parameter) const {
      xpp::render::tri_strip_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void tri_strip(Parameter && ... parameter) const {
      xpp::render::tri_strip(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void tri_fan_checked(Parameter && ... parameter) const {
      xpp::render::tri_fan_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void tri_fan(Parameter && ... parameter) const {
      xpp::render::tri_fan(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_glyph_set_checked(Parameter && ... parameter) const {
      xpp::render::create_glyph_set_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_glyph_set(Parameter && ... parameter) const {
      xpp::render::create_glyph_set(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void reference_glyph_set_checked(Parameter && ... parameter) const {
      xpp::render::reference_glyph_set_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void reference_glyph_set(Parameter && ... parameter) const {
      xpp::render::reference_glyph_set(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_glyph_set_checked(Parameter && ... parameter) const {
      xpp::render::free_glyph_set_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_glyph_set(Parameter && ... parameter) const {
      xpp::render::free_glyph_set(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_glyphs_checked(Parameter && ... parameter) const {
      xpp::render::add_glyphs_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add_glyphs(Parameter && ... parameter) const {
      xpp::render::add_glyphs(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void free_glyphs_checked(Parameter && ... parameter) const {
      xpp::render::free_glyphs_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void free_glyphs(Parameter && ... parameter) const {
      xpp::render::free_glyphs(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void composite_glyphs_8_checked(Parameter && ... parameter) const {
      xpp::render::composite_glyphs_8_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void composite_glyphs_8(Parameter && ... parameter) const {
      xpp::render::composite_glyphs_8(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void composite_glyphs_16_checked(Parameter && ... parameter) const {
      xpp::render::composite_glyphs_16_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void composite_glyphs_16(Parameter && ... parameter) const {
      xpp::render::composite_glyphs_16(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void composite_glyphs_32_checked(Parameter && ... parameter) const {
      xpp::render::composite_glyphs_32_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void composite_glyphs_32(Parameter && ... parameter) const {
      xpp::render::composite_glyphs_32(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void fill_rectangles_checked(Parameter && ... parameter) const {
      xpp::render::fill_rectangles_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void fill_rectangles(Parameter && ... parameter) const {
      xpp::render::fill_rectangles(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_cursor_checked(Parameter && ... parameter) const {
      xpp::render::create_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_cursor(Parameter && ... parameter) const {
      xpp::render::create_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_picture_transform_checked(Parameter && ... parameter) const {
      xpp::render::set_picture_transform_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_picture_transform(Parameter && ... parameter) const {
      xpp::render::set_picture_transform(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    auto query_filters(Parameter && ... parameter) const -> reply::checked::query_filters<Connection> {
      return xpp::render::query_filters(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    auto query_filters_unchecked(Parameter && ... parameter) const -> reply::unchecked::query_filters<Connection> {
      return xpp::render::query_filters_unchecked(
          connection(),
                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void set_picture_filter_checked(Parameter && ... parameter) const {
      xpp::render::set_picture_filter_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void set_picture_filter(Parameter && ... parameter) const {
      xpp::render::set_picture_filter(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_anim_cursor_checked(Parameter && ... parameter) const {
      xpp::render::create_anim_cursor_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_anim_cursor(Parameter && ... parameter) const {
      xpp::render::create_anim_cursor(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void add_traps_checked(Parameter && ... parameter) const {
      xpp::render::add_traps_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void add_traps(Parameter && ... parameter) const {
      xpp::render::add_traps(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_solid_fill_checked(Parameter && ... parameter) const {
      xpp::render::create_solid_fill_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_solid_fill(Parameter && ... parameter) const {
      xpp::render::create_solid_fill(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_linear_gradient_checked(Parameter && ... parameter) const {
      xpp::render::create_linear_gradient_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_linear_gradient(Parameter && ... parameter) const {
      xpp::render::create_linear_gradient(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_radial_gradient_checked(Parameter && ... parameter) const {
      xpp::render::create_radial_gradient_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_radial_gradient(Parameter && ... parameter) const {
      xpp::render::create_radial_gradient(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


    template<typename ... Parameter>
    void create_conical_gradient_checked(Parameter && ... parameter) const {
      xpp::render::create_conical_gradient_checked(connection(),
                                                    std::forward<Parameter>(parameter) ...);
    }

    template<typename ... Parameter>
    void create_conical_gradient(Parameter && ... parameter) const {
      xpp::render::create_conical_gradient(connection(),
                                    std::forward<Parameter>(parameter) ...);
    }


};

namespace event {

template<typename Connection>
class dispatcher {
  public:
    typedef xpp::render::extension extension;

    template<typename C>
    dispatcher(C && c, uint8_t first_event) : m_c(std::forward<C>(c)) , m_first_event(first_event) {}

    template<typename C>
    dispatcher(C && c, const xpp::render::extension & extension) : dispatcher(std::forward<C>(c), extension->first_event) {}

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
    typedef xpp::render::extension extension;

    dispatcher(uint8_t first_error) : m_first_error(first_error) {}

    dispatcher(const xpp::render::extension & extension) : dispatcher(extension->first_error) {}

    void operator()(const std::shared_ptr<xcb_generic_error_t> & error) const {
      switch (error->error_code - m_first_error) {

        case XCB_RENDER_PICT_FORMAT: // 0
          throw xpp::render::error::pict_format(error);

        case XCB_RENDER_PICTURE: // 1
          throw xpp::render::error::picture(error);

        case XCB_RENDER_PICT_OP: // 2
          throw xpp::render::error::pict_op(error);

        case XCB_RENDER_GLYPH_SET: // 3
          throw xpp::render::error::glyph_set(error);

        case XCB_RENDER_GLYPH: // 4
          throw xpp::render::error::glyph(error);

      };
    }

  protected:
    uint8_t m_first_error;
};

}


};

#endif // XPP_RENDER_HPP
