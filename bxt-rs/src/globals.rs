use crate::utils::RacyRefCell;

// If access is needed to multiple globals simultaneously, and the issue can't be solved with
// partial borrows, these globals can be split into their own RacyRefCells with corresponding
// methods on the MainThreadMarker.

/// Global variables. Access through [`MainThreadMarker::globals`].
///
/// [`MainThreadMarker::globals`]: ../utils/marker/struct.MainThreadMarker.html#method.globals
pub static GLOBALS: RacyRefCell<MainThreadGlobals> = RacyRefCell::new(MainThreadGlobals::new());

/// Global variables accessible from the main game thread.
pub struct MainThreadGlobals {}

impl MainThreadGlobals {
    #[inline]
    pub const fn new() -> Self {
        Self {}
    }
}
