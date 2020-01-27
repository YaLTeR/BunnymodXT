//! An interactive editor for TAS scripts.

use std::{
    fs::read_to_string, iter::once, os::raw::*, panic::catch_unwind, process::abort, str::FromStr,
};

use hltas::{types::Line, HLTAS};
use rental::rental;

use crate::{
    ffi::hlstrafe::{CurrentState, MovementVars, PlayerData, State0ms},
    triangle_api::{Primitive, RenderMode, TriangleApi},
    utils::MainThreadMarker,
};

pub mod simulator;
use simulator::Simulation;

// TODO: remove this by making HLTAS usable as an owned type by itself.
rental! {
    /// Module for the owned version of `TasEditor`.
    mod rentals {
        use super::*;

        /// A verion of `TasEditor` that doesn't need a `&'a str` to borrow from.
        #[rental]
        pub struct OwnedTasEditor {
            /// The owned input string.
            input: String,
            /// The TAS editor, borrowing from `input`.
            tas_editor: TasEditor<'input>,
        }
    }
}

pub use rentals::OwnedTasEditor;

/// An interactive editor for HLTAS input.
#[derive(Debug, Clone)]
pub struct TasEditor<'a> {
    /// The TAS being edited.
    hltas: HLTAS<'a>,
    /// The part of the TAS being edited and simulated.
    input: Input<'a>,
    /// Current mode.
    mode: Mode,
}

/// TAS editor modes.
#[derive(Debug, Clone, Copy, Eq, PartialEq)]
pub enum Mode {
    /// Append frame bulks and fly around.
    Append,
    /// Edit the existing input.
    Edit,
}

/// The part of the input currently being simulated.
#[derive(Debug, Clone)]
pub struct Input<'a> {
    /// The part of the input being edited and simulated.
    lines: Vec<Line<'a>>,
    /// Frame numbers of the first frame of each frame bulk, and the total frame count as the last
    /// entry.
    frame_bulk_starts: Vec<usize>,
    /// Frames that have already been simulated.
    ///
    /// The first entry is always the frame right before all frame bulks to simulate.
    frames: Vec<Frame>,
    /// The initial movement variables.
    initial_movement_vars: MovementVars,
}

/// Per-frame data.
#[derive(Debug, Copy, Clone, Default)]
pub struct Frame {
    /// The player state.
    player: PlayerData,
    /// Other HLStrafe state.
    state: CurrentState,
    /// Tracing fractions for the movement from from the previous frame to this one.
    fractions: [f32; 4],
    /// Collision normal Z components for the movement from from the previous frame to this one.
    normalzs: [f32; 4],
    /// Whether the next frame should be 0 ms.
    next_frame_is_0_ms: bool,
}

/// Which frame bulks to simulate?
#[derive(Debug, Copy, Clone, Eq, PartialEq)]
pub enum SimulateFrameBulks {
    /// Simulate all frame bulks.
    All,
    /// Simulate all frame bulks except the last one.
    AllExceptLast,
}

impl<'a> TasEditor<'a> {
    /// Creates a `TasEditor`.
    ///
    /// The TAS editor is set up to edit the script at `path` starting from `first_line` (this is
    /// in `hltas.lines`, basically non-empty lines in the `frames` section of the script, not in
    /// actual text lines), and from the current `player`, `state` and `movement_vars` with the
    /// `hlstrafe_version` in `mode`.
    ///
    /// # Panics
    ///
    /// Panics if `first_line` is higher than the number of lines in the HLTAS.
    pub fn new_owned(
        path: &str,
        first_line: usize,
        player: PlayerData,
        state: CurrentState,
        movement_vars: MovementVars,
        mode: Mode,
    ) -> OwnedTasEditor {
        let contents = read_to_string(path).unwrap(); // TODO: report error.
        OwnedTasEditor::new(contents, |input| {
            let hltas = HLTAS::from_str(input).unwrap();

            // Sanity check.
            if let Some(hlstrafe_version) = hltas.properties.hlstrafe_version {
                assert_eq!(hlstrafe_version.get(), state.Version);
            }

            let input = Input {
                lines: hltas.lines[first_line..].into(),
                frame_bulk_starts: vec![0],
                frames: vec![Frame {
                    player,
                    state,
                    fractions: [1., 0., 0., 0.],
                    normalzs: [0., 0., 0., 0.],
                    next_frame_is_0_ms: false, // TODO: set this properly?
                }],
                initial_movement_vars: movement_vars,
            };

            TasEditor { hltas, input, mode }
        })
    }

    /// Updates and draws the TAS editor.
    pub fn tick(
        &mut self,
        _marker: MainThreadMarker,
        tri: &TriangleApi,
        simulation: &mut Simulation,
    ) {
        self.input.simulate(simulation, SimulateFrameBulks::All);

        tri.render_mode(RenderMode::Normal);
        tri.color(1., 1., 1., 1.);
        tri.begin(Primitive::Lines);

        for frames in self.input.frames.windows(2) {
            let prev_frame = frames[0];
            let frame = frames[1];

            tri.vertex(prev_frame.player.Origin);
            tri.vertex(frame.player.Origin);
        }

        tri.end();
    }
}

impl<'a> Input<'a> {
    /// Simulates some number of not-yet-simulated frame bulks.
    fn simulate(&mut self, simulation: &mut Simulation, what: SimulateFrameBulks) {
        simulation.scope(true, |simulator| {
            let first_index = self.first_line_index();
            let last_index = self.last_line_index(what).max(first_index);
            let lines = &self.lines[first_index..last_index];
            if lines.is_empty() {
                return;
            }

            let mut movement_vars = self.initial_movement_vars;
            let Frame {
                mut player,
                mut state,
                mut next_frame_is_0_ms,
                ..
            } = *self.frames.last().unwrap();
            let mut frame_count = *self.frame_bulk_starts.last().unwrap();

            for line in lines {
                match line {
                    Line::FrameBulk(frame_bulk) => {
                        let host_frametime = f32::from_str(frame_bulk.frame_time).unwrap();
                        let frame_time = (host_frametime * 1000.).floor() * 0.001;

                        let first_frame = (self.frames.len() - 1 - frame_count) as u32;
                        for _ in first_frame..frame_bulk.frame_count.get() {
                            if next_frame_is_0_ms {
                                movement_vars.Frametime = 0.;
                            } else {
                                movement_vars.Frametime = frame_time;
                            }

                            let processed_frame =
                                simulator.simulate(&player, &movement_vars, line, &mut state);

                            player = processed_frame.NewPlayerData;
                            next_frame_is_0_ms = processed_frame.NextFrameIs0ms;

                            // PredictThis is needed because 0ms frames are batched client-side.
                            // Since we're re-using the HLStrafe prediction, here they are already
                            // predicted.
                            state.PredictThis = State0ms::NOTHING;

                            self.frames.push(Frame {
                                player,
                                state,
                                fractions: processed_frame.fractions,
                                normalzs: processed_frame.normalzs,
                                next_frame_is_0_ms,
                            });
                        }

                        frame_count += frame_bulk.frame_count.get() as usize;
                        self.frame_bulk_starts.push(frame_count);
                    }
                    // TODO
                    _ => (),
                }
            }
        });
    }

    /// Returns the index of the first line to simulate.
    fn first_line_index(&self) -> usize {
        // frame_bulk_starts always contains at least one element—zero.
        let first_frame_bulk_index = self.frame_bulk_starts.len() - 1;
        self.lines
            .iter()
            .enumerate()
            // If it's a frame bulk, return its index, otherwise skip.
            .filter_map(|(i, line)| {
                if let Line::FrameBulk(_) = line {
                    Some(i)
                } else {
                    None
                }
            })
            // Add the line count in the end, it will be returned if we simulated everything.
            .chain(once(self.lines.len()))
            .nth(first_frame_bulk_index)
            .unwrap()
    }

    /// Returns the index of one-past the last line to simulate.
    fn last_line_index(&self, what: SimulateFrameBulks) -> usize {
        match what {
            SimulateFrameBulks::All => self.lines.len(),
            SimulateFrameBulks::AllExceptLast => self
                .lines
                .iter()
                .enumerate()
                .rev()
                // Return the index of the first frame bulk from the end.
                .find_map(|(i, line)| {
                    if let Line::FrameBulk(_) = line {
                        Some(i)
                    } else {
                        None
                    }
                })
                .unwrap_or(0),
        }
    }
}

/// Initializes the TAS editor.
///
/// # Safety
///
/// `path` must be a valid pointer. This function must be called from the main game thread.
#[no_mangle]
pub unsafe extern "C" fn rs_create_tas_editor(
    path: *const c_char,
    first_line: c_uint,
    player: PlayerData,
    state: CurrentState,
    movement_vars: MovementVars,
    mode: c_uint,
) {
    let closure = move || {
        let marker = MainThreadMarker::new();
        let path = std::ffi::CStr::from_ptr(path).to_str().unwrap();
        let mode = match mode {
            1 => Mode::Append,
            2 => Mode::Edit,
            _ => panic!("invalid mode: {}", mode),
        };
        marker.globals_mut().tas_editor = Some(TasEditor::new_owned(
            path,
            first_line as _,
            player,
            state,
            movement_vars,
            mode,
        ));
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}

/// Drops the TAS editor.
///
/// # Safety
///
/// This function must be called from the main game thread.
#[no_mangle]
pub unsafe extern "C" fn rs_drop_tas_editor() {
    let closure = move || {
        let marker = MainThreadMarker::new();
        marker.globals_mut().tas_editor = None;
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}

#[cfg(test)]
mod tests {
    use hltas::types::FrameBulk;

    use super::*;

    fn default_movement_vars() -> MovementVars {
        MovementVars {
            Frametime: 0.01,
            Maxvelocity: 2000.,
            Maxspeed: 320.,
            Stopspeed: 100.,
            Friction: 4.,
            Edgefriction: 2.,
            EntFriction: 1.,
            Accelerate: 10.,
            Airaccelerate: 10.,
            Gravity: 800.,
            EntGravity: 1.,
            Stepsize: 18.,
            Bounce: 1.,
            Bhopcap: false,
        }
    }

    #[test]
    fn line_indices() {
        let input = Input {
            lines: vec![
                Line::Comment("hi"),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
            ],
            frame_bulk_starts: vec![0],
            frames: vec![Frame::default()],
            initial_movement_vars: default_movement_vars(),
        };

        assert_eq!(input.first_line_index(), 1);
        assert_eq!(input.last_line_index(SimulateFrameBulks::All), 2);
        assert_eq!(input.last_line_index(SimulateFrameBulks::AllExceptLast), 1);
    }

    #[test]
    fn line_indices_no_frame_bulks() {
        let input = Input {
            lines: vec![Line::Comment("hi"), Line::Comment("there")],
            frame_bulk_starts: vec![0],
            frames: vec![Frame::default()],
            initial_movement_vars: default_movement_vars(),
        };

        assert_eq!(input.first_line_index(), 2);
        assert_eq!(input.last_line_index(SimulateFrameBulks::All), 2);
        assert_eq!(input.last_line_index(SimulateFrameBulks::AllExceptLast), 0);
    }

    #[test]
    fn line_indices_some_simulated() {
        let input = Input {
            lines: vec![
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
            ],
            frame_bulk_starts: vec![0, 1, 2],
            frames: vec![Frame::default(); 3],
            initial_movement_vars: default_movement_vars(),
        };

        assert_eq!(input.first_line_index(), 2);
        assert_eq!(input.last_line_index(SimulateFrameBulks::All), 4);
        assert_eq!(input.last_line_index(SimulateFrameBulks::AllExceptLast), 3);
    }

    #[test]
    fn line_indices_all_simulated() {
        let input = Input {
            lines: vec![
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
                Line::FrameBulk(FrameBulk::with_frame_time("0.01")),
            ],
            frame_bulk_starts: vec![0, 1, 2, 3, 4],
            frames: vec![Frame::default(); 5],
            initial_movement_vars: default_movement_vars(),
        };

        assert_eq!(input.first_line_index(), 4);
        assert_eq!(input.last_line_index(SimulateFrameBulks::All), 4);
        assert_eq!(input.last_line_index(SimulateFrameBulks::AllExceptLast), 3);
    }
}
