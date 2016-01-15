
use std::fs::File;
use byteorder::{BigEndian, ReadBytesExt};
use std::io;
use std::io::{Error, ErrorKind, Seek, Read};

const HUNK_HEADER: u32 = 1011;
// hunk types
const HUNK_UNIT: u32 = 999;
const HUNK_NAME: u32 = 1000;
const HUNK_CODE: u32 = 1001;
const HUNK_DATA: u32 = 1002;
const HUNK_BSS: u32 = 1003;
const HUNK_RELOC32: u32 = 1004;
const HUNK_DEBUG: u32 = 1009;
const HUNK_SYMBOL: u32 = 1008;
const HUNK_END: u32 = 1010;

/*
   const HUNK_ABSRELOC32: u32 = HUNK_RELOC32;
   const HUNK_RELOC16: u32 = 1005;
   const HUNK_RELRELOC16 :u32 = HUNK_RELOC16;
   const HUNK_RELOC8: u32 = 1006;
   const HUNK_RELRELOC8: u32 = HUNK_RELOC8;

   const HUNK_OVERLAY: u32 = 1013;
   const HUNK_BREAK: u32 = 1014;

   const HUNK_DREL32: u32 = 1015;
   const HUNK_DREL16: u32 = 1016;
   const HUNK_DREL8: u32 = 1017;

   const HUNK_LIB: u32 = 1018;
   const HUNK_INDEX: u32 = 1019;
   */

/*
//
// Note: V37 LoadSeg uses 1015 (HUNK_DREL32) by mistake.  This will continue
// to be supported in future versions, since HUNK_DREL32 is illegal in load files
// anyways.  Future versions will support both 1015 and 1020, though anything
// that should be usable under V37 should use 1015.
//

const HUNK_RELOC32SHORT:u32 = 1020;

// see ext_xxx below.  New for V39 (note that LoadSeg only handles RELRELOC32).
const HUNK_RELRELOC32:u32 = 1021;
const HUNK_ABSRELOC16:u32 = 1022;

//
// Any hunks that have the HUNKB_ADVISORY bit set will be ignored if they
// aren't understood.  When ignored, they're treated like HUNK_DEBUG hunks.
// NOTE: this handling of HUNKB_ADVISORY started as of V39 dos.library!  If
// lading such executables is attempted under <V39 dos, it will fail with a
// bad hunk type.
//

const HUNKB_ADVISORY: u32 = 29;

// hunk_ext sub-type
const EXT_SYMB:u32 = 0; // table
const EXT_DEF: u32 = 1;	// relocatable definition
const EXT_ABS: u32 = 2;	// Absolute definition
const EXT_RES: u32 = 3;	// no longer supported
const EXT_REF32: u32 = 129;	// 32 bit absolute reference to symbol
const EXT_ABSREF32: u32 = EXT_REF32;
const EXT_COMMON: u32 = 130;	// 32 bit absolute reference to COMMON block
const EXT_ABSCOMMON: u32 = EXT_COMMON;
const EXT_REF16: u32 = 131;	// 16 bit PC-relative reference to symbol
const EXT_RELREF16: u32 = EXT_REF16;
const EXT_REF8: u32 = 132;	//  8 bit PC-relative reference to symbol
const EXT_RELREF8: u32 = EXT_REF8;
const EXT_DEXT32: u32 = 133; // 32 bit data relative reference
const EXT_DEXT16: u32 = 134; // 16 bit data relative reference
const EXT_DEXT8: u32 = 135; // 8 bit data relative reference

// These are to support some of the '020 and up modes that are rarely used
const EXT_RELREF32:u32 = 136;	// 32 bit PC-relative reference to symbol
const EXT_RELCOMMON:u32 = 137;	// 32 bit PC-relative reference to COMMON block

// for completeness... All 680x0's support this
const EXT_ABSREF16:u32 = 138;	// 16 bit absolute reference to symbol

// this only exists on '020's and above, in the (d8,An,Xn) address mode
const EXT_ABSREF8:u32 = 139; //8 bit absolute reference to symbol

*/

const HUNKF_CHIP: u32 = 1 << 30;
const HUNKF_FAST: u32 = 1 << 31;

pub struct HunkParser;


#[derive(Clone, Copy, Debug)]
enum HunkType {
    Code,
    Data,
    Bss,
}

pub struct RelocInfo32 {
    pub target: usize, 
    pub data: Vec<u32>,
}

pub struct Hunk {
    mem_type: MemoryType,
    hunk_type: HunkType,
    alloc_size: usize,
    data_size: usize,
    code_data: Option<Vec<u8>>, 
    reloc_32: Option<Vec<RelocInfo32>>,
}

#[derive(Clone, Copy, Debug)]
enum MemoryType {
    Any,
    Chip,
    Fast,
}

struct SizesTypes {
    mem_type: MemoryType,
    size: usize,
}

impl HunkParser {
    fn skip_hunk(file: &mut File, name: &str) -> io::Result<()> {
        println!("Skipping {}\n", name);
        let seek_offset = try!(file.read_u32::<BigEndian>());
        file.seek(io::SeekFrom::Current(seek_offset as i64)).map(|_|())
    }

    fn get_size_type(t: u32) -> (usize, MemoryType) {
        let size = (t & 0x0fffffff) * 4;
        let mem_t = t & 0xf0000000;
        let mem_type = match mem_t {
            HUNKF_CHIP => MemoryType::Chip,
            HUNKF_FAST => MemoryType::Fast,
            _ => MemoryType::Any,
        };

        (size as usize, mem_type)
    }

    fn parse_bss(hunk: &mut Hunk, file: &mut File) -> io::Result<()> {
        let (size, mem_type) = Self::get_size_type(try!(file.read_u32::<BigEndian>()));
        hunk.hunk_type = HunkType::Bss;
        hunk.data_size = size;
        hunk.mem_type = mem_type;
        Ok(())
    }

    fn parse_code_or_data(hunk_type: HunkType, hunk: &mut Hunk, file: &mut File) -> io::Result<()> {
        let (size, mem_type) = Self::get_size_type(try!(file.read_u32::<BigEndian>()));
        let mut code_data: Vec<u8> = vec![0; size];

        hunk.data_size = size;
        hunk.hunk_type = hunk_type;
        hunk.mem_type = mem_type;

        try!(file.read(&mut code_data));

        hunk.code_data = Some(code_data);

        Ok(())
    }

    fn parse_symbol(file: &mut File) -> io::Result<()> {
        let mut sym_len = try!(file.read_u32::<BigEndian>()) * 4;

        while sym_len > 0 {
            try!(file.seek(io::SeekFrom::Current((sym_len + 4) as i64)).map(|_| ()));
            sym_len = try!(file.read_u32::<BigEndian>()) * 4;
        }

        Ok(())
    }

    fn parse_reloc32(hunk: &mut Hunk, file: &mut File) -> io::Result<()> {
        let mut relocs = Vec::<RelocInfo32>::new();  

        loop {
            let count = try!(file.read_u32::<BigEndian>()) as usize;

            if count == 0 {
                break;
            }

            let target = try!(file.read_u32::<BigEndian>()) as usize;

            let mut reloc = RelocInfo32 {
                target: target,
                data: Vec::<u32>::with_capacity(count),
            };

            for _ in 0..count {
                reloc.data.push(try!(file.read_u32::<BigEndian>()));
            }

            relocs.push(reloc);
        }

        hunk.reloc_32 = Some(relocs);

        Ok(())
    }

    fn fill_hunk(hunk: &mut Hunk, file: &mut File) -> io::Result<()> {
        loop {
            let hunk_type = try!(file.read_u32::<BigEndian>());

            match hunk_type {
                HUNK_UNIT => { try!(Self::skip_hunk(file, "HUNK_UNIT")) }
                HUNK_NAME => { try!(Self::skip_hunk(file, "HUNK_NAME")) }
                HUNK_DEBUG => { try!(Self::skip_hunk(file, "HUNK_DEBUG")) }
                HUNK_CODE => { try!(Self::parse_code_or_data(HunkType::Code, hunk, file)) }
                HUNK_DATA => { try!(Self::parse_code_or_data(HunkType::Data, hunk, file)) }
                HUNK_BSS => { try!(Self::parse_bss(hunk, file)) }
                HUNK_RELOC32 => { try!(Self::parse_reloc32(hunk, file)) }
                HUNK_SYMBOL => { try!(Self::parse_symbol(file)) }
                HUNK_END => {
                    return Ok(());
                }

                _ => {
                    return Err(Error::new(ErrorKind::Other, "Unsupported hunk"));
                }
            }
        }
    }

    pub fn parse_file(filename: &str) -> io::Result<()> {
        //let mut index = 0;

        let mut file = try!(File::open(filename));

        let hunk_header = try!(file.read_u32::<BigEndian>());
        if hunk_header != HUNK_HEADER  {
            return Err(Error::new(ErrorKind::Other, "Unable to find correct HUNK_HEADER"));
        };

        // Skip header/string section
        try!(file.read_u32::<BigEndian>());

        let table_size = try!(file.read_u32::<BigEndian>()) as i32;
        let first_hunk = try!(file.read_u32::<BigEndian>()) as i32;
        let last_hunk = try!(file.read_u32::<BigEndian>()) as i32;

        if table_size < 0 || first_hunk < 0 || last_hunk < 0 {
            return Err(Error::new(ErrorKind::Other, "Invalid sizes for hunks"));
        }

        let hunk_count = (last_hunk - first_hunk + 1) as usize;

        let mut hunk_table = Vec::with_capacity(hunk_count);

        for _ in 0..hunk_count {
            let (size, mem_type) = Self::get_size_type(try!(file.read_u32::<BigEndian>()));
            hunk_table.push(SizesTypes {
                mem_type: mem_type,
                size: size 
            });
        }

        let mut hunks = Vec::with_capacity(hunk_count);

        for i in 0..hunk_count {
            let mut hunk = Hunk {
                mem_type: hunk_table[i].mem_type, 
                    hunk_type: HunkType::Bss,
                    alloc_size: hunk_table[i].size as usize,
                    data_size: 0,
                    code_data: None, 
                    reloc_32: None, 
            };

            try!(Self::fill_hunk(&mut hunk, &mut file));

            hunks.push(hunk);
        }

        // dump info

        for hunk in hunks {
            println!("type {:?} - size {} - alloc_size {}", hunk.hunk_type, hunk.data_size, hunk.alloc_size); 
        }

        //println!("b {}", hunk_header);
        Ok(())
    }
}
