#![allow(unused, non_snake_case, non_upper_case_globals, non_camel_case_types)]

use std::{
    mem::{align_of, size_of},
    os::raw::*,
    ptr::null,
};

use hltas_cpp_bridge::hltas_cpp;
use ultraviolet::Vec3;

#[repr(C)]
#[derive(Debug, Copy, Clone, Default)]
pub struct PlayerData {
    pub Origin: Vec3,
    pub Velocity: Vec3,
    pub Basevelocity: Vec3,
    pub Viewangles: Vec3,
    pub Ducking: bool,
    pub InDuckAnimation: bool,
    pub DuckTime: f32,
    pub HasLJModule: bool,
    pub SpeedBeforeFriction: f64,
}

#[cfg(all(target_arch = "x86", target_family = "unix"))]
#[test]
fn bindgen_test_layout_PlayerData() {
    assert_eq!(
        size_of::<PlayerData>(),
        68usize,
        concat!("Size of: ", stringify!(PlayerData))
    );
    assert_eq!(
        align_of::<PlayerData>(),
        4usize,
        concat!("Alignment of ", stringify!(PlayerData))
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Origin as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Origin)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Velocity as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Velocity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Basevelocity as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Basevelocity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Viewangles as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Viewangles)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Ducking as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Ducking)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).InDuckAnimation as *const _ as usize },
        49usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(InDuckAnimation)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).DuckTime as *const _ as usize },
        52usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(DuckTime)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).HasLJModule as *const _ as usize },
        56usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(HasLJModule)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).SpeedBeforeFriction as *const _ as usize },
        60usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(SpeedBeforeFriction)
        )
    );
}

#[cfg(all(target_arch = "x86", target_family = "windows"))]
#[test]
fn bindgen_test_layout_PlayerData() {
    assert_eq!(
        size_of::<PlayerData>(),
        72usize,
        concat!("Size of: ", stringify!(PlayerData))
    );
    assert_eq!(
        align_of::<PlayerData>(),
        8usize,
        concat!("Alignment of ", stringify!(PlayerData))
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Origin as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Origin)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Velocity as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Velocity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Basevelocity as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Basevelocity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Viewangles as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Viewangles)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).Ducking as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(Ducking)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).InDuckAnimation as *const _ as usize },
        49usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(InDuckAnimation)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).DuckTime as *const _ as usize },
        52usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(DuckTime)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).HasLJModule as *const _ as usize },
        56usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(HasLJModule)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<PlayerData>())).SpeedBeforeFriction as *const _ as usize },
        64usize,
        concat!(
            "Offset of field: ",
            stringify!(PlayerData),
            "::",
            stringify!(SpeedBeforeFriction)
        )
    );
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct MovementVars {
    pub Frametime: f32,
    pub Maxvelocity: f32,
    pub Maxspeed: f32,
    pub Stopspeed: f32,
    pub Friction: f32,
    pub Edgefriction: f32,
    pub EntFriction: f32,
    pub Accelerate: f32,
    pub Airaccelerate: f32,
    pub Gravity: f32,
    pub EntGravity: f32,
    pub Stepsize: f32,
    pub Bounce: f32,
    pub Bhopcap: bool,
}

#[cfg(all(target_arch = "x86", target_family = "unix"))]
#[test]
fn bindgen_test_layout_MovementVars() {
    assert_eq!(
        size_of::<MovementVars>(),
        56usize,
        concat!("Size of: ", stringify!(MovementVars))
    );
    assert_eq!(
        align_of::<MovementVars>(),
        4usize,
        concat!("Alignment of ", stringify!(MovementVars))
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Frametime as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Frametime)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Maxvelocity as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Maxvelocity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Maxspeed as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Maxspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Stopspeed as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Stopspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Friction as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Friction)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Edgefriction as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Edgefriction)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).EntFriction as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(EntFriction)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Accelerate as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Accelerate)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Airaccelerate as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Airaccelerate)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Gravity as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Gravity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).EntGravity as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(EntGravity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Stepsize as *const _ as usize },
        44usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Stepsize)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Bounce as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Bounce)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Bhopcap as *const _ as usize },
        52usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Bhopcap)
        )
    );
}

#[cfg(all(target_arch = "x86", target_family = "windows"))]
#[test]
fn bindgen_test_layout_MovementVars() {
    assert_eq!(
        size_of::<MovementVars>(),
        56usize,
        concat!("Size of: ", stringify!(MovementVars))
    );
    assert_eq!(
        align_of::<MovementVars>(),
        4usize,
        concat!("Alignment of ", stringify!(MovementVars))
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Frametime as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Frametime)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Maxvelocity as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Maxvelocity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Maxspeed as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Maxspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Stopspeed as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Stopspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Friction as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Friction)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Edgefriction as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Edgefriction)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).EntFriction as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(EntFriction)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Accelerate as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Accelerate)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Airaccelerate as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Airaccelerate)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Gravity as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Gravity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).EntGravity as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(EntGravity)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Stepsize as *const _ as usize },
        44usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Stepsize)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Bounce as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Bounce)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<MovementVars>())).Bhopcap as *const _ as usize },
        52usize,
        concat!(
            "Offset of field: ",
            stringify!(MovementVars),
            "::",
            stringify!(Bhopcap)
        )
    );
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct ProcessedFrame {
    pub Pitch: f32,
    pub Yaw: f32,
    pub Forward: bool,
    pub Left: bool,
    pub Right: bool,
    pub Back: bool,
    pub Up: bool,
    pub Down: bool,
    pub Jump: bool,
    pub Duck: bool,
    pub Use: bool,
    pub Attack1: bool,
    pub Attack2: bool,
    pub Reload: bool,
    pub Forwardspeed: f32,
    pub Sidespeed: f32,
    pub Backspeed: f32,
    pub Upspeed: f32,
    pub NextFrameIs0ms: bool,
    pub NewPlayerData: PlayerData,
    pub fractions: [f32; 4usize],
    pub normalzs: [f32; 4usize],
}

#[cfg(all(target_arch = "x86", target_family = "unix"))]
#[test]
fn bindgen_test_layout_ProcessedFrame() {
    assert_eq!(
        size_of::<ProcessedFrame>(),
        140usize,
        concat!("Size of: ", stringify!(ProcessedFrame))
    );
    assert_eq!(
        align_of::<ProcessedFrame>(),
        4usize,
        concat!("Alignment of ", stringify!(ProcessedFrame))
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Pitch as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Pitch)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Yaw as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Yaw)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Forward as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Forward)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Left as *const _ as usize },
        9usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Left)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Right as *const _ as usize },
        10usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Right)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Back as *const _ as usize },
        11usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Back)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Up as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Up)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Down as *const _ as usize },
        13usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Down)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Jump as *const _ as usize },
        14usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Jump)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Duck as *const _ as usize },
        15usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Duck)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Use as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Use)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Attack1 as *const _ as usize },
        17usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Attack1)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Attack2 as *const _ as usize },
        18usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Attack2)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Reload as *const _ as usize },
        19usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Reload)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Forwardspeed as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Forwardspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Sidespeed as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Sidespeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Backspeed as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Backspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Upspeed as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Upspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).NextFrameIs0ms as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(NextFrameIs0ms)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).NewPlayerData as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(NewPlayerData)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).fractions as *const _ as usize },
        108usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(fractions)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).normalzs as *const _ as usize },
        124usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(normalzs)
        )
    );
}

#[cfg(all(target_arch = "x86", target_family = "windows"))]
#[test]
fn bindgen_test_layout_ProcessedFrame() {
    assert_eq!(
        size_of::<ProcessedFrame>(),
        144usize,
        concat!("Size of: ", stringify!(ProcessedFrame))
    );
    assert_eq!(
        align_of::<ProcessedFrame>(),
        8usize,
        concat!("Alignment of ", stringify!(ProcessedFrame))
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Pitch as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Pitch)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Yaw as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Yaw)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Forward as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Forward)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Left as *const _ as usize },
        9usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Left)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Right as *const _ as usize },
        10usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Right)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Back as *const _ as usize },
        11usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Back)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Up as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Up)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Down as *const _ as usize },
        13usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Down)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Jump as *const _ as usize },
        14usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Jump)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Duck as *const _ as usize },
        15usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Duck)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Use as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Use)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Attack1 as *const _ as usize },
        17usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Attack1)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Attack2 as *const _ as usize },
        18usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Attack2)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Reload as *const _ as usize },
        19usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Reload)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Forwardspeed as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Forwardspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Sidespeed as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Sidespeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Backspeed as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Backspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).Upspeed as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(Upspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).NextFrameIs0ms as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(NextFrameIs0ms)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).NewPlayerData as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(NewPlayerData)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).fractions as *const _ as usize },
        112usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(fractions)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<ProcessedFrame>())).normalzs as *const _ as usize },
        128usize,
        concat!(
            "Offset of field: ",
            stringify!(ProcessedFrame),
            "::",
            stringify!(normalzs)
        )
    );
}

#[repr(i32)]
#[derive(Debug, Copy, Clone, Eq, PartialEq)]
pub enum State0ms {
    NOTHING = 0,
    DUCKED = 1,
    UNDUCKED = 2,
    UNDUCKED_AND_DUCKED = 3,
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct CurrentState {
    pub Jump: bool,
    pub Duck: bool,
    pub JumpbugsLeft: c_uint,
    pub Ducktap0ms: bool,
    pub DucktapsLeft: c_uint,
    pub AutojumpsLeft: c_uint,
    pub DbcCeilings: bool,
    pub DbcsLeft: c_uint,
    pub DbgsLeft: c_uint,
    pub DwjsLeft: c_uint,
    pub LgagstFullMaxspeed: bool,
    pub LgagstType: bool,
    pub LgagstMinSpeed: f32,
    pub LgagstsLeft: c_uint,
    pub ButtonsPresent: bool,
    pub Buttons: hltas_cpp::StrafeButtons,
    pub PredictThis: State0ms,
    pub Algorithm: hltas_cpp::StrafingAlgorithm,
    pub Parameters: hltas_cpp::AlgorithmParameters,
    pub LastVelocity: [f32; 2usize],
}

impl Default for CurrentState {
    #[inline]
    fn default() -> Self {
        Self {
            Jump: false,
            Duck: false,
            JumpbugsLeft: 0,
            Ducktap0ms: false,
            DucktapsLeft: 0,
            AutojumpsLeft: 0,
            DbcCeilings: false,
            DbcsLeft: 0,
            DbgsLeft: 0,
            DwjsLeft: 0,
            LgagstFullMaxspeed: false,
            LgagstType: false,
            LgagstMinSpeed: 30.,
            LgagstsLeft: 0,
            ButtonsPresent: false,
            Buttons: hltas_cpp::StrafeButtons::default(),
            PredictThis: State0ms::NOTHING,
            Algorithm: hltas_cpp::StrafingAlgorithm::default(),
            Parameters: hltas_cpp::AlgorithmParameters::default(),
            LastVelocity: [0.; 2],
        }
    }
}

#[cfg(all(target_arch = "x86", target_family = "unix"))]
#[test]
fn bindgen_test_layout_CurrentState() {
    assert_eq!(
        size_of::<CurrentState>(),
        92usize,
        concat!("Size of: ", stringify!(CurrentState))
    );
    assert_eq!(
        align_of::<CurrentState>(),
        4usize,
        concat!("Alignment of ", stringify!(CurrentState))
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Jump as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Jump)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Duck as *const _ as usize },
        1usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Duck)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).JumpbugsLeft as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(JumpbugsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Ducktap0ms as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Ducktap0ms)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DucktapsLeft as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DucktapsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).AutojumpsLeft as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(AutojumpsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DbcCeilings as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DbcCeilings)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DbcsLeft as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DbcsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DbgsLeft as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DbgsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DwjsLeft as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DwjsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstFullMaxspeed as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstFullMaxspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstType as *const _ as usize },
        37usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstType)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstMinSpeed as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstMinSpeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstsLeft as *const _ as usize },
        44usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).ButtonsPresent as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(ButtonsPresent)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Buttons as *const _ as usize },
        49usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Buttons)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).PredictThis as *const _ as usize },
        56usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(PredictThis)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Algorithm as *const _ as usize },
        60usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Algorithm)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Parameters as *const _ as usize },
        64usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Parameters)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LastVelocity as *const _ as usize },
        84usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LastVelocity)
        )
    );
}

#[cfg(all(target_arch = "x86", target_family = "windows"))]
#[test]
fn bindgen_test_layout_CurrentState() {
    assert_eq!(
        size_of::<CurrentState>(),
        88usize,
        concat!("Size of: ", stringify!(CurrentState))
    );
    assert_eq!(
        align_of::<CurrentState>(),
        8usize,
        concat!("Alignment of ", stringify!(CurrentState))
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Jump as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Jump)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Duck as *const _ as usize },
        1usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Duck)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).JumpbugsLeft as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(JumpbugsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Ducktap0ms as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Ducktap0ms)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DucktapsLeft as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DucktapsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).AutojumpsLeft as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(AutojumpsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DbcCeilings as *const _ as usize },
        20usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DbcCeilings)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DbcsLeft as *const _ as usize },
        24usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DbcsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DbgsLeft as *const _ as usize },
        28usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DbgsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).DwjsLeft as *const _ as usize },
        32usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(DwjsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstFullMaxspeed as *const _ as usize },
        36usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstFullMaxspeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstType as *const _ as usize },
        37usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstType)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstMinSpeed as *const _ as usize },
        40usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstMinSpeed)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LgagstsLeft as *const _ as usize },
        44usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LgagstsLeft)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).PredictThis as *const _ as usize },
        48usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(PredictThis)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Algorithm as *const _ as usize },
        52usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Algorithm)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).Parameters as *const _ as usize },
        56usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(Parameters)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<CurrentState>())).LastVelocity as *const _ as usize },
        80usize,
        concat!(
            "Offset of field: ",
            stringify!(CurrentState),
            "::",
            stringify!(LastVelocity)
        )
    );
}

extern "C" {
    pub fn bxt_start_simulating(extend_distance_limit: c_int);
    pub fn bxt_simulate(
        player: *const PlayerData,
        vars: *const MovementVars,
        frame: *const hltas_cpp::hltas_frame,
        curState: *mut CurrentState,
        version: c_uint,
    ) -> ProcessedFrame;
    pub fn bxt_stop_simulating();
}
