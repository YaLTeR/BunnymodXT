use std::cell::{Ref, RefCell, RefMut};

/// An unsafe `RefCell` with `Send` and `Sync`, intended to be used as a `static` and accessible
/// from a single thread only.
pub struct RacyRefCell<T>(RefCell<T>);

unsafe impl<T> Send for RacyRefCell<T> {}
unsafe impl<T> Sync for RacyRefCell<T> {}

impl<T> RacyRefCell<T> {
    /// Creates a new `RacyRefCell` containing `value`.
    #[inline]
    pub const fn new(value: T) -> Self {
        RacyRefCell(RefCell::new(value))
    }

    /// Immutably borrows the wrapped value.
    ///
    /// # Safety
    /// This must be called from a single thread only.
    #[inline]
    pub unsafe fn borrow(&self) -> Ref<T> {
        self.0.borrow()
    }

    /// Mutably borrows the wrapped value.
    ///
    /// # Safety
    /// This must be called from a single thread only.
    #[inline]
    pub unsafe fn borrow_mut(&self) -> RefMut<T> {
        self.0.borrow_mut()
    }
}
