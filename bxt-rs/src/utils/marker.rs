//! The main thread marker.

use std::{
    cell::{Ref, RefMut},
    marker::PhantomData,
};

use crate::{
    cvar::{CVars, CVARS},
    globals::{MainThreadGlobals, GLOBALS},
    server::{Server, SERVER},
};

/// This marker serves as a static guarantee of being on the main game thread. Functions that
/// should only be called from the main game thread should accept an argument of this type.
#[derive(Clone, Copy)]
pub struct MainThreadMarker {
    // Mark as !Send and !Sync.
    _marker: PhantomData<*const ()>,
}

impl MainThreadMarker {
    /// Creates a new `MainThreadMarker`.
    ///
    /// # Safety
    /// This should only be called from the main game thread.
    #[inline]
    pub unsafe fn new() -> Self {
        Self {
            _marker: PhantomData,
        }
    }

    /// Returns an immutable reference to [`MainThreadGlobals`].
    ///
    /// [`MainThreadGlobals`]: ../../globals/struct.MainThreadGlobals.html
    #[inline]
    pub fn globals(self) -> Ref<'static, MainThreadGlobals> {
        // We know we're on the main thread because we accept self which is a MainThreadMarker.
        unsafe { GLOBALS.borrow() }
    }

    /// Returns a mutable reference to [`MainThreadGlobals`].
    ///
    /// [`MainThreadGlobals`]: ../../globals/struct.MainThreadGlobals.html
    #[inline]
    pub fn globals_mut(self) -> RefMut<'static, MainThreadGlobals> {
        // We know we're on the main thread because we accept self which is a MainThreadMarker.
        unsafe { GLOBALS.borrow_mut() }
    }

    /// Returns an immutable reference to [`Server`].
    ///
    /// [`Server`]: ../../server/struct.Server.html
    #[inline]
    pub fn server(self) -> Ref<'static, Server> {
        // We know we're on the main thread because we accept self which is a MainThreadMarker.
        unsafe { SERVER.borrow() }
    }

    /// Returns a mutable reference to [`Server`].
    ///
    /// [`Server`]: ../../server/struct.Server.html
    #[inline]
    pub fn server_mut(self) -> RefMut<'static, Server> {
        // We know we're on the main thread because we accept self which is a MainThreadMarker.
        unsafe { SERVER.borrow_mut() }
    }

    /// Returns an immutable reference to [`CVars`].
    ///
    /// [`CVars`]: ../../cvar/struct.CVars.html
    #[inline]
    pub fn cvars(self) -> Ref<'static, CVars> {
        // We know we're on the main thread because we accept self which is a MainThreadMarker.
        unsafe { CVARS.borrow() }
    }

    /// Returns a mutable reference to [`CVars`].
    ///
    /// [`CVars`]: ../../cvar/struct.CVars.html
    #[inline]
    pub fn cvars_mut(self) -> RefMut<'static, CVars> {
        // We know we're on the main thread because we accept self which is a MainThreadMarker.
        unsafe { CVARS.borrow_mut() }
    }
}
