export module i3:shape;

import :x;

export class Shape {
   public:
    int shape_base{-1};
    bool shape_supported{true};

    explicit Shape(X *x) {
        x_connection *conn = *x;
        auto shape_ext = conn->extension<xpp::shape::extension>();
        if (shape_ext->present) {
            shape_base = shape_ext->first_event;
            auto version = x->conn->shape().query_version();
            shape_supported = version && version->minor_version >= 1;
        } else {
            shape_supported = false;
        }
        if (!shape_supported) {
            //DLOG("shape 1.1 is not present on this server\n");
        }
    }
};
