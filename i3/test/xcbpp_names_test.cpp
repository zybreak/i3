/*
 * i3's code names xcbpp's constants and types where it named libxcb's (MIGRATION-PLAN.md §4.3). This compile-time test
 * checks that every constant it replaced has libxcb's value, and that every type it respelled is libxcb's type, so
 * that the new names changed no request and no comparison.
 */
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <xcb/shape.h>

import std;
import xcb;
import xcb.proto.randr;
import xcb.proto.shape;

/* xcbpp's RandR module has constants of these macros' names. */
constexpr auto libxcb_randr_major_version = XCB_RANDR_MAJOR_VERSION;
constexpr auto libxcb_randr_minor_version = XCB_RANDR_MINOR_VERSION;
#undef XCB_RANDR_MAJOR_VERSION
#undef XCB_RANDR_MINOR_VERSION

static_assert(std::is_same_v<xcb::xproto::window_t, xcb_window_t>);
static_assert(std::is_same_v<xcb::xproto::atom_t, xcb_atom_t>);
static_assert(std::is_same_v<xcb::xproto::timestamp_t, xcb_timestamp_t>);
static_assert(std::is_same_v<xcb::xproto::cursor_t, xcb_cursor_t>);
static_assert(std::is_same_v<xcb::xproto::colormap_t, xcb_colormap_t>);
static_assert(std::is_same_v<xcb::xproto::visualid_t, xcb_visualid_t>);
static_assert(std::is_same_v<xcb::xproto::keysym_t, xcb_keysym_t>);
static_assert(std::is_same_v<xcb::xproto::keycode_t, xcb_keycode_t>);
static_assert(std::is_same_v<xcb::randr::randr_output_t, xcb_randr_output_t>);
static_assert(std::is_same_v<xcb::randr::randr_crtc_t, xcb_randr_crtc_t>);

static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_ATOM) == XCB_ATOM_ATOM);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_CARDINAL) == XCB_ATOM_CARDINAL);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_NONE) == XCB_ATOM_NONE);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_STRING) == XCB_ATOM_STRING);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_SUPERSCRIPT_X) == XCB_ATOM_SUPERSCRIPT_X);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WINDOW) == XCB_ATOM_WINDOW);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_CLASS) == XCB_ATOM_WM_CLASS);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_CLIENT_MACHINE) == XCB_ATOM_WM_CLIENT_MACHINE);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_HINTS) == XCB_ATOM_WM_HINTS);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_NAME) == XCB_ATOM_WM_NAME);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_NORMAL_HINTS) == XCB_ATOM_WM_NORMAL_HINTS);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_SIZE_HINTS) == XCB_ATOM_WM_SIZE_HINTS);
static_assert(std::to_underlying(xcb::xproto::atom_enum_t::ATOM_WM_TRANSIENT_FOR) == XCB_ATOM_WM_TRANSIENT_FOR);
static_assert(std::to_underlying(xcb::xproto::button_index_t::BUTTON_INDEX_1) == XCB_BUTTON_INDEX_1);
static_assert(std::to_underlying(xcb::xproto::button_index_t::BUTTON_INDEX_2) == XCB_BUTTON_INDEX_2);
static_assert(std::to_underlying(xcb::xproto::button_index_t::BUTTON_INDEX_3) == XCB_BUTTON_INDEX_3);
static_assert(std::to_underlying(xcb::xproto::button_index_t::BUTTON_INDEX_4) == XCB_BUTTON_INDEX_4);
static_assert(std::to_underlying(xcb::xproto::button_index_t::BUTTON_INDEX_5) == XCB_BUTTON_INDEX_5);
static_assert(xcb::xproto::BUTTON_PRESS == XCB_BUTTON_PRESS);
static_assert(xcb::xproto::BUTTON_RELEASE == XCB_BUTTON_RELEASE);
static_assert(xcb::xproto::CLIENT_MESSAGE == XCB_CLIENT_MESSAGE);
static_assert(xcb::xproto::CONFIGURE_NOTIFY == XCB_CONFIGURE_NOTIFY);
static_assert(xcb::xproto::CONFIGURE_REQUEST == XCB_CONFIGURE_REQUEST);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_BORDER_WIDTH) == XCB_CONFIG_WINDOW_BORDER_WIDTH);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_HEIGHT) == XCB_CONFIG_WINDOW_HEIGHT);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_SIBLING) == XCB_CONFIG_WINDOW_SIBLING);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_STACK_MODE) == XCB_CONFIG_WINDOW_STACK_MODE);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_WIDTH) == XCB_CONFIG_WINDOW_WIDTH);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_X) == XCB_CONFIG_WINDOW_X);
static_assert(std::to_underlying(xcb::xproto::config_window_t::CONFIG_WINDOW_Y) == XCB_CONFIG_WINDOW_Y);
static_assert(xcb::copy_from_parent == XCB_COPY_FROM_PARENT);
static_assert(xcb::current_time == XCB_CURRENT_TIME);
static_assert(xcb::xproto::DESTROY_NOTIFY == XCB_DESTROY_NOTIFY);
static_assert(xcb::xproto::ENTER_NOTIFY == XCB_ENTER_NOTIFY);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_BUTTON_PRESS) == XCB_EVENT_MASK_BUTTON_PRESS);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_BUTTON_RELEASE) == XCB_EVENT_MASK_BUTTON_RELEASE);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_ENTER_WINDOW) == XCB_EVENT_MASK_ENTER_WINDOW);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_EXPOSURE) == XCB_EVENT_MASK_EXPOSURE);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_FOCUS_CHANGE) == XCB_EVENT_MASK_FOCUS_CHANGE);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_POINTER_MOTION) == XCB_EVENT_MASK_POINTER_MOTION);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_PROPERTY_CHANGE) == XCB_EVENT_MASK_PROPERTY_CHANGE);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_STRUCTURE_NOTIFY) == XCB_EVENT_MASK_STRUCTURE_NOTIFY);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_SUBSTRUCTURE_NOTIFY) == XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY);
static_assert(std::to_underlying(xcb::xproto::event_mask_t::EVENT_MASK_SUBSTRUCTURE_REDIRECT) == XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT);
static_assert(xcb::xproto::EXPOSE == XCB_EXPOSE);
static_assert(xcb::xproto::FOCUS_IN == XCB_FOCUS_IN);
static_assert(xcb::xproto::FOCUS_OUT == XCB_FOCUS_OUT);
static_assert(std::to_underlying(xcb::xproto::get_property_type_t::GET_PROPERTY_TYPE_ANY) == XCB_GET_PROPERTY_TYPE_ANY);
static_assert(std::to_underlying(xcb::xproto::grab_t::GRAB_ANY) == XCB_GRAB_ANY);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_BUTTON_1) == XCB_KEY_BUT_MASK_BUTTON_1);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_BUTTON_2) == XCB_KEY_BUT_MASK_BUTTON_2);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_BUTTON_3) == XCB_KEY_BUT_MASK_BUTTON_3);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_BUTTON_4) == XCB_KEY_BUT_MASK_BUTTON_4);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_BUTTON_5) == XCB_KEY_BUT_MASK_BUTTON_5);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_CONTROL) == XCB_KEY_BUT_MASK_CONTROL);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_LOCK) == XCB_KEY_BUT_MASK_LOCK);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_1) == XCB_KEY_BUT_MASK_MOD_1);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_2) == XCB_KEY_BUT_MASK_MOD_2);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_3) == XCB_KEY_BUT_MASK_MOD_3);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_4) == XCB_KEY_BUT_MASK_MOD_4);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_5) == XCB_KEY_BUT_MASK_MOD_5);
static_assert(std::to_underlying(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_SHIFT) == XCB_KEY_BUT_MASK_SHIFT);
static_assert(xcb::xproto::KEY_PRESS == XCB_KEY_PRESS);
static_assert(xcb::xproto::KEY_RELEASE == XCB_KEY_RELEASE);
static_assert(std::to_underlying(xcb::xproto::mapping_t::MAPPING_KEYBOARD) == XCB_MAPPING_KEYBOARD);
static_assert(std::to_underlying(xcb::xproto::mapping_t::MAPPING_MODIFIER) == XCB_MAPPING_MODIFIER);
static_assert(xcb::xproto::MAPPING_NOTIFY == XCB_MAPPING_NOTIFY);
static_assert(xcb::xproto::MAP_REQUEST == XCB_MAP_REQUEST);
static_assert(std::to_underlying(xcb::xproto::mod_mask_t::MOD_MASK_LOCK) == XCB_MOD_MASK_LOCK);
static_assert(xcb::xproto::MOTION_NOTIFY == XCB_MOTION_NOTIFY);
static_assert(xcb::none == XCB_NONE);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_ANCESTOR) == XCB_NOTIFY_DETAIL_ANCESTOR);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_INFERIOR) == XCB_NOTIFY_DETAIL_INFERIOR);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_NONE) == XCB_NOTIFY_DETAIL_NONE);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_NONLINEAR) == XCB_NOTIFY_DETAIL_NONLINEAR);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_NONLINEAR_VIRTUAL) == XCB_NOTIFY_DETAIL_NONLINEAR_VIRTUAL);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_POINTER) == XCB_NOTIFY_DETAIL_POINTER);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_POINTER_ROOT) == XCB_NOTIFY_DETAIL_POINTER_ROOT);
static_assert(std::to_underlying(xcb::xproto::notify_detail_t::NOTIFY_DETAIL_VIRTUAL) == XCB_NOTIFY_DETAIL_VIRTUAL);
static_assert(std::to_underlying(xcb::xproto::notify_mode_t::NOTIFY_MODE_GRAB) == XCB_NOTIFY_MODE_GRAB);
static_assert(std::to_underlying(xcb::xproto::notify_mode_t::NOTIFY_MODE_NORMAL) == XCB_NOTIFY_MODE_NORMAL);
static_assert(std::to_underlying(xcb::xproto::notify_mode_t::NOTIFY_MODE_UNGRAB) == XCB_NOTIFY_MODE_UNGRAB);
static_assert(std::to_underlying(xcb::xproto::notify_mode_t::NOTIFY_MODE_WHILE_GRABBED) == XCB_NOTIFY_MODE_WHILE_GRABBED);
static_assert(std::to_underlying(xcb::xproto::pixmap_enum_t::PIXMAP_NONE) == XCB_PIXMAP_NONE);
static_assert(std::to_underlying(xcb::xproto::property_t::PROPERTY_DELETE) == XCB_PROPERTY_DELETE);
static_assert(xcb::xproto::PROPERTY_NOTIFY == XCB_PROPERTY_NOTIFY);
static_assert(xcb::randr::XCB_RANDR_MAJOR_VERSION == libxcb_randr_major_version);
static_assert(xcb::randr::XCB_RANDR_MINOR_VERSION == libxcb_randr_minor_version);
static_assert(xcb::randr::RANDR_SCREEN_CHANGE_NOTIFY == XCB_RANDR_SCREEN_CHANGE_NOTIFY);
static_assert(xcb::xproto::SELECTION_CLEAR == XCB_SELECTION_CLEAR);
static_assert(xcb::shape::SHAPE_NOTIFY == XCB_SHAPE_NOTIFY);
static_assert(std::to_underlying(xcb::shape::shape_sk_t::SHAPE_SK_BOUNDING) == XCB_SHAPE_SK_BOUNDING);
static_assert(std::to_underlying(xcb::shape::shape_sk_t::SHAPE_SK_INPUT) == XCB_SHAPE_SK_INPUT);
static_assert(std::to_underlying(xcb::xproto::stack_mode_t::STACK_MODE_ABOVE) == XCB_STACK_MODE_ABOVE);
static_assert(xcb::xproto::UNMAP_NOTIFY == XCB_UNMAP_NOTIFY);
static_assert(std::to_underlying(xcb::xproto::window_class_t::WINDOW_CLASS_INPUT_ONLY) == XCB_WINDOW_CLASS_INPUT_ONLY);
static_assert(std::to_underlying(xcb::xproto::window_class_t::WINDOW_CLASS_INPUT_OUTPUT) == XCB_WINDOW_CLASS_INPUT_OUTPUT);
static_assert(std::to_underlying(xcb::xproto::window_enum_t::WINDOW_NONE) == XCB_WINDOW_NONE);
