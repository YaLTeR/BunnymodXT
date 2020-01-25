use std::{panic::catch_unwind, process::abort};

use crate::{ffi::triangle_api as ffi, globals::MainThreadMarker, triangle_api::*};

fn draw_nodes(marker: MainThreadMarker, tri: &TriangleApi) {
    tri.render_mode(RenderMode::TransAdd);
    tri.color(0.722, 0., 0.341, 1.);

    for position in marker.server().node_positions() {
        tri.pyramid(position, 10., 30.);
    }
}

/// Does all triangle rendering.
///
/// # Safety
///
/// The function must be called from the main game thread.
///
/// `tri_api` must be valid, its contents must be valid and not change during this function, and
/// this function must be called when it's safe to use the triangle API.
#[no_mangle]
pub unsafe extern "C" fn rs_tri_draw(tri_api: *const ffi::triangleapi_s) {
    let closure = move || {
        let marker = MainThreadMarker::new();

        let tri_api = tri_api.as_ref().unwrap();
        let tri = TriangleApi::new(tri_api);

        draw_nodes(marker, &tri);
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}
