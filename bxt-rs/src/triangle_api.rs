#![allow(unused)]

use ultraviolet::{Vec2, Vec3};

use crate::ffi::triangle_api as ffi;

pub struct TriangleApi<'a> {
    api: &'a ffi::triangleapi_s,
}

#[repr(i32)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum Primitive {
    Triangles = ffi::TRI_TRIANGLES,
    TriangleFan = ffi::TRI_TRIANGLE_FAN,
    Quads = ffi::TRI_QUADS,
    Polygon = ffi::TRI_POLYGON,
    Lines = ffi::TRI_LINES,
    TriangleStrip = ffi::TRI_TRIANGLE_STRIP,
    QuadStrip = ffi::TRI_QUAD_STRIP,
}

#[repr(i32)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum CullStyle {
    Front = ffi::TRICULLSTYLE_TRI_FRONT,
    None = ffi::TRICULLSTYLE_TRI_NONE,
}

#[repr(i32)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum RenderMode {
    /// `src`
    Normal = ffi::kRenderNormal,
    /// `c*a + dest*(1-a)`
    TransColor = ffi::kRenderTransColor,
    /// `src*a + dest*(1-a)`
    TransTexture = ffi::kRenderTransTexture,
    /// `src*a + dest` -- no Z buffer checks
    Glow = ffi::kRenderGlow,
    /// `src*srca + dest*(1-srca)`
    TransAlpha = ffi::kRenderTransAlpha,
    /// `src*a + dest`
    TransAdd = ffi::kRenderTransAdd,
}

impl<'a> TriangleApi<'a> {
    /// Wraps the FFI `triangleapi_s`.
    ///
    /// # Safety
    ///
    /// All pointers in `api` must be valid, and the API must be safe to use over the lifetime of
    /// this struct.
    pub unsafe fn new(api: &'a ffi::triangleapi_s) -> Self {
        Self { api }
    }

    pub fn begin(&self, primitive: Primitive) {
        unsafe { (self.api.Begin)(primitive as _) }
    }

    pub fn end(&self) {
        unsafe { (self.api.End)() }
    }

    pub fn render_mode(&self, mode: RenderMode) {
        unsafe { (self.api.RenderMode)(mode as _) }
    }

    pub fn cull(&self, style: CullStyle) {
        unsafe { (self.api.CullFace)(style as _) }
    }

    pub fn color(&self, r: f32, g: f32, b: f32, a: f32) {
        unsafe { (self.api.Color4f)(r, g, b, a) }
    }

    pub fn vertex(&self, v: Vec3) {
        unsafe { (self.api.Vertex3fv)(v.as_ptr()) }
    }

    pub fn screen_to_world(&self, screen: Vec2) -> Vec3 {
        let screen = Vec3::from(screen);
        let mut world = Vec3::zero();
        unsafe { (self.api.ScreenToWorld)(screen.as_ptr(), world.as_mut_ptr()) }
        world
    }
}
