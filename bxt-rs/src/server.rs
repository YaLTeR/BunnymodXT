use std::{os::raw::c_int, panic::catch_unwind, process::abort, ptr::null};

use ultraviolet::Vec3;

use crate::{globals::MainThreadMarker, utils::racy_ref_cell::RacyRefCell};

/// The server global. Access through [`MainThreadMarker::server`].
///
/// [`MainThreadMarker::server`]: ../globals/struct.MainThreadMarker.html#method.server
// The Server invariants are upheld for this global.
pub static SERVER: RacyRefCell<Server> = RacyRefCell::new(Server::new());

pub struct Server {
    // Invariant: either null or valid to get nodes from.
    world_graph: *const Graph,
}

#[repr(C)]
pub struct Graph {
    _private: [u8; 0],
}

impl Server {
    const fn new() -> Self {
        Self {
            world_graph: null(),
        }
    }

    pub fn node_positions(&self) -> Vec<Vec3> {
        const OFF_M_C_NODES: usize = 0x18;
        const OFF_M_P_NODES: usize = 0x0C;
        const OFF_M_VEC_ORIGIN: usize = 0x00;
        const SIZE_C_NODE: usize = 0x58;

        if self.world_graph.is_null() {
            return vec![];
        }

        // Assumption: when world_graph isn't null, it's valid to extract nodes from.
        unsafe {
            let count = *((self.world_graph as usize + OFF_M_C_NODES) as *const c_int);
            let mut node = *((self.world_graph as usize + OFF_M_P_NODES) as *const usize);

            let mut positions = Vec::with_capacity(count as usize);
            for _ in 0..count {
                positions.push(*((node + OFF_M_VEC_ORIGIN) as *const Vec3));
                node += SIZE_C_NODE;
            }

            positions
        }
    }
}

/// Sets the `WorldGraph` pointer.
///
/// # Safety
///
/// The function must be called from the main game thread. The pointer must be valid and the graph
/// must be ready to extract nodes from.
#[no_mangle]
pub unsafe extern "C" fn rs_init_graph(world_graph: *const Graph) {
    let closure = move || {
        let marker = MainThreadMarker::new();
        marker.server_mut().world_graph = world_graph;
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}

/// Resets the server structure.
///
/// # Safety
///
/// The function must be called from the main game thread.
#[no_mangle]
pub unsafe extern "C" fn rs_server_clear() {
    let closure = move || {
        let marker = MainThreadMarker::new();
        *marker.server_mut() = Server::new();
    };

    if catch_unwind(closure).is_err() {
        abort();
    }
}
