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

    fn clear_memory<'a>(dest: &'a mut [u8], size: usize) {
        for i in 0..size {
            dest[i] = 0;
        }
    }

    pub fn load_code_data(hunks: &mut Vec<Hunk>, hunk_type: HunkType, offset: usize) -> usize {
        let mut curr_offset = offset;

        for hunk in hunks.iter_mut() {
            if hunk.hunk_type == hunk_type {
                hunk.memory_offset = curr_offset;
                hunk.code_data.as_ref().map(|data| {
                    unsafe {
                        // Needs to be unsafe because Musashi callbacks doesn't provide userdata
                        // thus the Amiga memory needs to be global
                        Self::copy_data(&data, &mut AMIGA_MEMORY[curr_offset..], hunk.data_size);
                    }

                    curr_offset += hunk.data_size;
                });
            }
        }

        curr_offset
    }

    fn init_bss_sections(hunks: &mut Vec<Hunk>, offset: usize) -> usize {
        let mut curr_offset = offset;

        for hunk in hunks.iter_mut() {
            if hunk.hunk_type == HunkType::Bss {
                hunk.memory_offset = curr_offset;
                hunk.code_data.as_ref().map(|_| {
                    unsafe {
                        // Needs to be unsafe because Musashi callbacks doesn't provide userdata
                        // thus the Amiga memory needs to be global
                        Self::clear_memory(&mut AMIGA_MEMORY[curr_offset..], hunk.data_size);
                    }

                    curr_offset += hunk.data_size;
                });
            }
        }

        curr_offset
    }

    fn load_to_memory(hunks: &mut Vec<Hunk>) {
        let code_end = Self::load_code_data(hunks, HunkType::Code, EXE_START);
        let data_end = Self::load_code_data(hunks, HunkType::Data, code_end);
        let bss_end = Self::init_bss_sections(hunks, data_end);

        println!("code end: {} data end: {} bss end: {}", code_end, data_end, bss_end);

        let mut pc = EXE_START as u32;

        while pc < code_end as u32 {
            pc += Musashi::disassemble(pc);
        }
    }

    pub fn load_executable_to_memory(&self, filename: &str) -> io::Result<()> {
        let mut hunks = Self::parse_executable(self, filename)?;

        /*
        for hunk in &hunks {
           println!("type {:?} - size {} - alloc_size {}", hunk.hunk_type, hunk.data_size, hunk.alloc_size);
        }
        */

        Self::load_to_memory(&mut hunks);

        for hunk in &hunks {
            println!("type {:?} - {:?}", hunk.hunk_type, hunk);
        }

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
