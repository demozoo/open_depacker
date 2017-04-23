use super::byteorder::{BigEndian, ByteOrder};
use super::amiga_hunk_parser::{HunkParser, Hunk, HunkType};
use super::musashi::Musashi;
use std::io;

// Memory layout of the 'Amiga'
// 0 - 512K is reserved for the "rom"
// 512 - 4MB - Executable has the range of 16MB
// 4MB - 16MB - 16MB -> Heap, bss, etc

static mut AMIGA_MEMORY: [u8; 16 * 1024 * 1024] = [0; 16 * 1024 * 1024];

const EXE_START: usize = 512 * 1024;

pub struct Amiga {
    pub dummy: i32,
}

impl Amiga {
    pub fn new() -> Amiga {
        Amiga { dummy: 0 }
    }

    pub fn parse_executable(&self, filename: &str) -> Result<Vec<Hunk>, io::Error> {
        HunkParser::parse_file(filename)
    }

    fn copy_data<'a>(src: &'a [u8], dest: &'a mut [u8], size: usize) {
        for i in 0..size {
            dest[i] = src[i];
        }
    }

    fn load_to_memory(hunks: Vec<Hunk>) {
        let mut exe_pos = EXE_START;
        let mut total_code_size = 0u32;

        for hunk in &hunks {
            if hunk.hunk_type == HunkType::Code {
                hunk.code_data
                    .as_ref()
                    .map(|data| {
                        unsafe {
                            Self::copy_data(&data, &mut AMIGA_MEMORY[exe_pos..], hunk.data_size);
                        }
                        exe_pos += hunk.data_size;
                        total_code_size += hunk.data_size as u32;
                    });
            }
        }

        let mut pc = EXE_START as u32;

        println!("total code size {}", total_code_size);

        while pc < EXE_START as u32 + total_code_size {
            pc += Musashi::disassemble(pc);
        }
    }

    pub fn load_executable_to_memory(&self, filename: &str) -> io::Result<()> {
        let hunks = try!(Self::parse_executable(self, filename));

        /*
        for hunk in &hunks {
           println!("type {:?} - size {} - alloc_size {}", hunk.hunk_type, hunk.data_size, hunk.alloc_size); 
        }
        */

        Self::load_to_memory(hunks);

        Ok(())
    }
}

//
// This is kinda ugly but this is the way Musashi works. When we have
// our own 68k emulator this should be cleaned up and not be global
//

#[no_mangle]
pub extern "C" fn m68k_read_memory_32(address: u32) -> u32 {
    unsafe { BigEndian::read_u32(&AMIGA_MEMORY[address as usize..]) }
}

#[no_mangle]
pub extern "C" fn m68k_read_memory_16(address: u32) -> u16 {
    unsafe { BigEndian::read_u16(&AMIGA_MEMORY[address as usize..]) as u16 }
}

#[no_mangle]
pub extern "C" fn m68k_read_memory_8(address: u32) -> u32 {
    unsafe { AMIGA_MEMORY[address as usize] as u32 }
}

#[no_mangle]
pub extern "C" fn m68k_write_memory_8(address: u32, value: u32) {
    unsafe { AMIGA_MEMORY[address as usize] = value as u8 }
}

#[no_mangle]
pub extern "C" fn m68k_write_memory_16(address: u32, value: u32) {
    unsafe { BigEndian::write_u16(&mut AMIGA_MEMORY[address as usize..], value as u16) }
}

#[no_mangle]
pub extern "C" fn m68k_write_memory_32(address: u32, value: u32) {
    unsafe { BigEndian::write_u32(&mut AMIGA_MEMORY[address as usize..], value) }
}

#[no_mangle]
pub extern "C" fn m68k_read_disassembler_32(address: u32) -> u32 {
    m68k_read_memory_32(address)
}

#[no_mangle]
pub extern "C" fn m68k_read_disassembler_16(address: u32) -> u32 {
    m68k_read_memory_16(address) as u32
}

#[no_mangle]
pub extern "C" fn m68k_read_disassembler_8(address: u32) -> u32 {
    m68k_read_memory_8(address)
}
