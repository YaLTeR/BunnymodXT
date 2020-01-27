//! Player movement simulation.

use std::{marker::PhantomData, mem::ManuallyDrop, num::NonZeroU32};

use hltas::types::Line;
use hltas_cpp_bridge::hltas_frame_from_non_comment_line;

use crate::ffi::hlstrafe as ffi;

/// Allows to create simulation scopes.
pub struct Simulation {
    // Mark as !Send and !Sync.
    _marker: PhantomData<*const ()>,
}

/// A simulation scope.
pub struct Simulator {
    // Mark as !Send and !Sync.
    _marker: PhantomData<*const ()>,
}

impl Simulation {
    /// Creates a new `Simulation`.
    ///
    /// # Safety
    ///
    /// It must be safe to call the BXT simulation functions while this object is alive. Only one
    /// of these objects must be alive at the same time.
    pub unsafe fn new() -> Self {
        Self {
            _marker: PhantomData,
        }
    }

    /// Creates a simulation scope and invokes a closure, passing the [`Simulator`] to it.
    ///
    /// If `extend_distance_limit` is set to `true`, removes the default player movement tracing
    /// limit.
    ///
    /// [`Simulator`]: struct.Simulator.html
    pub fn scope(&mut self, extend_distance_limit: bool, f: impl FnOnce(&Simulator)) {
        unsafe {
            ffi::bxt_start_simulating(if extend_distance_limit { 1 } else { 0 });
        }

        f(&Simulator {
            _marker: PhantomData,
        });

        unsafe {
            ffi::bxt_stop_simulating();
        }
    }
}

impl Simulator {
    /// Simulates a frame bulk line.
    ///
    /// # Panics
    ///
    /// Panics if `line` is not a `Line::FrameBulk`.
    pub fn simulate(
        &self,
        player: &ffi::PlayerData,
        vars: &ffi::MovementVars,
        line: &Line,
        cur_state: &mut ffi::CurrentState,
        version: NonZeroU32,
    ) -> ffi::ProcessedFrame {
        match line {
            Line::FrameBulk(_) => (),
            _ => panic!("line is not a frame bulk: {:#?}", line),
        };

        unsafe {
            let (frame, mut strings) = hltas_frame_from_non_comment_line(line);
            let processed_frame = ffi::bxt_simulate(player, vars, &frame, cur_state, version.get());
            ManuallyDrop::drop(&mut strings);
            processed_frame
        }
    }
}
