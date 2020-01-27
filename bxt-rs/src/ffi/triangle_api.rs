#![allow(unused, non_snake_case, non_upper_case_globals)]

use std::{
    mem::{align_of, size_of},
    os::raw::*,
    ptr::null,
};

pub const TRI_API_VERSION: c_int = 1;

pub const TRI_TRIANGLES: c_int = 0;
pub const TRI_TRIANGLE_FAN: c_int = 1;
pub const TRI_QUADS: c_int = 2;
pub const TRI_POLYGON: c_int = 3;
pub const TRI_LINES: c_int = 4;
pub const TRI_TRIANGLE_STRIP: c_int = 5;
pub const TRI_QUAD_STRIP: c_int = 6;

pub const kRenderNormal: c_int = 0;
pub const kRenderTransColor: c_int = 1;
pub const kRenderTransTexture: c_int = 2;
pub const kRenderGlow: c_int = 3;
pub const kRenderTransAlpha: c_int = 4;
pub const kRenderTransAdd: c_int = 5;

pub const TRICULLSTYLE_TRI_FRONT: TRICULLSTYLE = 0;
pub const TRICULLSTYLE_TRI_NONE: TRICULLSTYLE = 1;
pub type TRICULLSTYLE = c_int;

#[repr(C)]
pub struct model_s {
    _private: [u8; 0],
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct triangleapi_s {
    pub version: c_int,

    pub RenderMode: unsafe extern "C" fn(mode: c_int),
    pub Begin: unsafe extern "C" fn(primitive: c_int),
    pub End: unsafe extern "C" fn(),
    pub Color4f: unsafe extern "C" fn(r: f32, g: f32, b: f32, a: f32),
    pub Color4ub: unsafe extern "C" fn(r: c_uchar, g: c_uchar, b: c_uchar, a: c_uchar),
    pub TexCoord2f: unsafe extern "C" fn(u: f32, v: f32),
    pub Vertex3fv: unsafe extern "C" fn(worldPnt: *const f32),
    pub Vertex3f: unsafe extern "C" fn(x: f32, y: f32, z: f32),
    pub Brightness: unsafe extern "C" fn(brightness: f32),
    pub CullFace: unsafe extern "C" fn(style: TRICULLSTYLE),
    pub SpriteTexture: unsafe extern "C" fn(pSpriteModel: *const model_s, frame: c_int) -> c_int,
    pub WorldToScreen: unsafe extern "C" fn(world: *const f32, screen: *mut f32) -> c_int,
    pub Fog: unsafe extern "C" fn(flFogColor: *const f32, flStart: f32, flEnd: f32, bOn: c_int),
    pub ScreenToWorld: unsafe extern "C" fn(screen: *const f32, world: *mut f32),
    pub GetMatrix: unsafe extern "C" fn(pname: c_int, matrix: *mut f32),
    pub BoxInPVS: unsafe extern "C" fn(mins: *const f32, maxs: *const f32) -> c_int,
    pub LightAtPoint: unsafe extern "C" fn(pos: *const f32, value: *mut f32),
    pub Color4fRendermode: unsafe extern "C" fn(r: f32, g: f32, b: f32, a: f32, rendermode: c_int),
    pub FogParams: unsafe extern "C" fn(flDensity: f32, iFogSkybox: c_int),
}

#[cfg(target_arch = "x86")]
#[test]
fn bindgen_test_layout_triangleapi_s() {
    assert_eq!(
        size_of::<triangleapi_s>(),
        80usize,
        concat!("Size of: ", stringify!(triangleapi_s))
    );
    assert_eq!(
        align_of::<triangleapi_s>(),
        4usize,
        concat!("Alignment of ", stringify!(triangleapi_s))
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).version as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(version)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).RenderMode as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(RenderMode)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Begin as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Begin)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).End as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(End)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Color4f as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Color4f)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Color4ub as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Color4ub)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).TexCoord2f as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(TexCoord2f)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Vertex3fv as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Vertex3fv)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Vertex3f as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Vertex3f)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Brightness as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Brightness)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).CullFace as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(CullFace)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).SpriteTexture as *const _ as usize },
        44usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(SpriteTexture)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).WorldToScreen as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(WorldToScreen)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Fog as *const _ as usize },
        52usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Fog)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).ScreenToWorld as *const _ as usize },
        56usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(ScreenToWorld)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).GetMatrix as *const _ as usize },
        60usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(GetMatrix)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).BoxInPVS as *const _ as usize },
        64usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(BoxInPVS)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).LightAtPoint as *const _ as usize },
        68usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(LightAtPoint)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).Color4fRendermode as *const _ as usize },
        72usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(Color4fRendermode)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<triangleapi_s>())).FogParams as *const _ as usize },
        76usize,
        concat!(
            "Offset of field: ",
            stringify!(triangleapi_s),
            "::",
            stringify!(FogParams)
        )
    );
}
