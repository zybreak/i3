#include <xcb/xcb.h>
#include <xpp/xpp.hpp>
#include "atoms.h"
#include "global.h"

/* Define all atoms as global variables */
#define xmacro(atom) xcb_atom_t A_##atom;
    I3_NET_SUPPORTED_ATOMS_XMACRO
    I3_REST_ATOMS_XMACRO
#undef xmacro

std::array<xcb_atom_t, 35> supported_atoms[] = {
#define xmacro(atom) {A_##atom},
    I3_NET_SUPPORTED_ATOMS_XMACRO
#undef xmacro
};

/* Setup NetWM atoms */
void setup_atoms() {
#define xmacro(name) \
    try {                 \
        A_##name = xpp::x::intern_atom(*global.a, false, #name).atom(); \
    } catch (std::exception &e) { \
        ELOG("Could not get atom " #name "\n");                                        \
        exit(-1);                                                                      \
    }
    I3_NET_SUPPORTED_ATOMS_XMACRO
    I3_REST_ATOMS_XMACRO
#undef xmacro
}