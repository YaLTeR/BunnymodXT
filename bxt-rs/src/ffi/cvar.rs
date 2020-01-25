#![allow(unused)]

use std::{
    mem::{align_of, size_of},
    os::raw::*,
    ptr::null,
};

use bitflags::bitflags;

bitflags! {
    pub struct Flags: i32 {
        const FCVAR_ARCHIVE = 1;
        const FCVAR_USERINFO = 2;
        const FCVAR_SERVER = 4;
        const FCVAR_EXTDLL = 8;
        const FCVAR_CLIENTDLL = 16;
        const FCVAR_PROTECTED = 32;
        const FCVAR_SPONLY = 64;
        const FCVAR_PRINTABLEONLY = 128;
        const FCVAR_UNLOGGED = 256;
    }
}

#[repr(C)]
#[derive(Debug)]
pub struct cvar_s {
    pub name: *mut c_char,
    pub string: *mut c_char,
    pub flags: c_int,
    pub value: f32,
    pub next: *mut cvar_s,
}

#[test]
fn bindgen_test_layout_cvar_s() {
    assert_eq!(
        size_of::<cvar_s>(),
        20usize,
        concat!("Size of: ", stringify!(cvar_s))
    );
    assert_eq!(
        align_of::<cvar_s>(),
        4usize,
        concat!("Alignment of ", stringify!(cvar_s))
    );
    assert_eq!(
        unsafe { &(*(null::<cvar_s>())).name as *const _ as usize },
        0usize,
        concat!(
            "Offset of field: ",
            stringify!(cvar_s),
            "::",
            stringify!(name)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<cvar_s>())).string as *const _ as usize },
        4usize,
        concat!(
            "Offset of field: ",
            stringify!(cvar_s),
            "::",
            stringify!(string)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<cvar_s>())).flags as *const _ as usize },
        8usize,
        concat!(
            "Offset of field: ",
            stringify!(cvar_s),
            "::",
            stringify!(flags)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<cvar_s>())).value as *const _ as usize },
        12usize,
        concat!(
            "Offset of field: ",
            stringify!(cvar_s),
            "::",
            stringify!(value)
        )
    );
    assert_eq!(
        unsafe { &(*(null::<cvar_s>())).next as *const _ as usize },
        16usize,
        concat!(
            "Offset of field: ",
            stringify!(cvar_s),
            "::",
            stringify!(next)
        )
    );
}
