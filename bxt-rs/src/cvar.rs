use std::{ffi::CStr, os::raw::c_char, panic::catch_unwind, process::abort, ptr::null};

use crate::{ffi::cvar::*, utils::MainThreadMarker, utils::RacyRefCell};

/// Console variables. Access through [`MainThreadMarker::cvars`].
///
/// [`MainThreadMarker::cvars`]: ../utils/marker/struct.MainThreadMarker.html#method.cvars
// The CVar invariants are upheld for this global.
pub static CVARS: RacyRefCell<CVars> = RacyRefCell::new(CVars::new());

pub struct CVar {
    // Invariant: if this is not null, it's safe to access string and value.
    ptr: *const cvar_s,
}

pub struct CVars {
    pub bxt_show_nodes: CVar,
}

impl CVar {
    const fn new() -> Self {
        Self { ptr: null() }
    }

    /// Returns a bool value of this cvar.
    pub fn bool(&self) -> bool {
        if self.ptr.is_null() {
            return false;
        }

        unsafe { (*self.ptr).value != 0. }
    }
}

impl CVars {
    const fn new() -> Self {
        Self {
            bxt_show_nodes: CVar::new(),
        }
    }
}

/// Initializes the cvar pointer.
///
/// # Safety
///
/// The function must be called from the main game thread.
///
/// The pointers must be valid, and the cvar must be safe to access starting from this point and
/// over the duration of the program.
#[no_mangle]
pub unsafe extern "C" fn rs_init_cvar(name: *const c_char, ptr: *const cvar_s) {
    let closure = move || {
        let marker = MainThreadMarker::new();
        let mut cvars = marker.cvars_mut();

        let name = CStr::from_ptr(name).to_str().unwrap();
        let cvar = match name {
            "bxt_show_nodes" => &mut cvars.bxt_show_nodes,
            _ => return,
        };

        cvar.ptr = ptr;
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}
