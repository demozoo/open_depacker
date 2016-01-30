extern crate byteorder;

mod ffi;

pub struct Musashi {
    pub dummy: i32,
}

impl Musashi {
    pub fn init() {
        //unsafe { 
            //m68k_init();
        //}
    }

    pub fn disassemble(pc: u32) -> u32 {
        unsafe {
            ffi::m68k_disassemble(0 as *mut i8, pc, 1) as u32
        }
    }
}


