//! Primitives for drawing in 3D space.
#![allow(unused)]

use ultraviolet::{Vec2, Vec3};

use crate::ffi::triangle_api as ffi;

/// Triangle API wrapper for drawing in 3D space.
pub struct TriangleApi<'a> {
    /// Raw API.
    api: &'a ffi::triangleapi_s,
}

/// Primitive types which determine how vertices are drawn.
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

/// Polygon culling style.
#[repr(i32)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum CullStyle {
    /// Only the back face is drawn, defined as clockwise vertices.
    Front = ffi::TRICULLSTYLE_TRI_FRONT,
    /// Both front and back faces are drawn.
    None = ffi::TRICULLSTYLE_TRI_NONE,
}

/// Compositing mode.
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

    /// Starts the specified primitive drawing mode.
    pub fn begin(&self, primitive: Primitive) {
        unsafe { (self.api.Begin)(primitive as _) }
    }

    /// Stops the current primitive drawing mode.
    pub fn end(&self) {
        unsafe { (self.api.End)() }
    }

    /// Sets the render mode.
    pub fn render_mode(&self, mode: RenderMode) {
        unsafe { (self.api.RenderMode)(mode as _) }
    }

    /// Sets the cull style.
    pub fn cull(&self, style: CullStyle) {
        unsafe { (self.api.CullFace)(style as _) }
    }

    /// Sets the vertex color.
    pub fn color(&self, r: f32, g: f32, b: f32, a: f32) {
        unsafe { (self.api.Color4f)(r, g, b, a) }
    }

    /// Adds a vertex at the given position.
    pub fn vertex(&self, position: Vec3) {
        unsafe { (self.api.Vertex3fv)(position.as_ptr()) }
    }

    /// Converts screen coordinates to world coordinates.
    ///
    /// The screen coordinate system ranges from -1 to 1 where (-1, -1) is the bottom-left corner
    /// of the screen.
    pub fn screen_to_world(&self, screen: Vec2) -> Vec3 {
        let screen = Vec3::from(screen);
        let mut world = Vec3::zero();
        unsafe { (self.api.ScreenToWorld)(screen.as_ptr(), world.as_mut_ptr()) }
        world
    }

    /// Draws a pyramid.
    pub fn pyramid(&self, position: Vec3, width: f32, height: f32) {
        let half_width = width * 0.5;
        let bottom = [
            position + Vec3::new(half_width, half_width, 0.),
            position + Vec3::new(half_width, -half_width, 0.),
            position + Vec3::new(-half_width, -half_width, 0.),
            position + Vec3::new(-half_width, half_width, 0.),
            position + Vec3::new(half_width, half_width, 0.),
        ];

        self.cull(CullStyle::Front);

        self.begin(Primitive::Quads);
        for &v in bottom[..4].iter().rev() {
            self.vertex(v);
        }
        self.end();

        self.begin(Primitive::TriangleFan);
        self.vertex(position + Vec3::new(0., 0., height));
        for &v in &bottom {
            self.vertex(v);
        }
        self.end();
    }
}
