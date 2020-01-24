use std::{panic::catch_unwind, process::abort};

use ultraviolet::Vec2;

use crate::{ffi::triangle_api as ffi, triangle_api::*};

/// Does all triangle rendering.
///
/// # Safety
///
/// `tri_api` must be valid, its contents must be valid and not change during this function, and
/// this function must be called when it's safe to use the triangle API.
#[no_mangle]
pub unsafe extern "C" fn rs_tri_draw(tri_api: *const ffi::triangleapi_s) {
    let closure = move || {
        let tri_api = tri_api.as_ref().unwrap();
        let tri = TriangleApi::new(tri_api);

        tri.render_mode(RenderMode::TransColor);
        tri.color(0., 1., 0., 1.);
        tri.begin(Primitive::Triangles);
        tri.vertex(tri.screen_to_world(Vec2::new(-0.5, 0.5)));
        tri.vertex(tri.screen_to_world(Vec2::new(0.5, 0.)));
        tri.vertex(tri.screen_to_world(Vec2::new(0., 0.5)));
        tri.end();
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}
