
use std::fs::File;
use byteorder::{BigEndian, ReadBytesExt};
use std::io;
use std::io::{Error, ErrorKind};

const HUNK_HEADER: u32 = 1011;
/*
// hunk types
const HUNK_UNIT: u32 = 999;
const HUNK_NAME: u32 = 1000;
const HUNK_CODE: u32 = 1001;
const HUNK_DATA: u32 = 1002;
const HUNK_BSS: u32 = 1003;
const HUNK_RELOC32: u32 = 1004;
const HUNK_ABSRELOC32: u32 = HUNK_RELOC32;
const HUNK_RELOC16: u32 = 1005;
const HUNK_RELRELOC16 :u32 = HUNK_RELOC16;
const HUNK_RELOC8: u32 = 1006;
const HUNK_RELRELOC8: u32 = HUNK_RELOC8;
const HUNK_EXT: u32 = 1007;
const HUNK_SYMBOL: u32 = 1008;
const HUNK_DEBUG: u32 = 1009;
const HUNK_END: u32 = 1010;

const HUNK_OVERLAY: u32 = 1013;
const HUNK_BREAK: u32 = 1014;

const HUNK_DREL32: u32 = 1015;
const HUNK_DREL16: u32 = 1016;
const HUNK_DREL8: u32 = 1017;

const HUNK_LIB: u32 = 1018;
const HUNK_INDEX: u32 = 1019;

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
const HUNKB_CHIP: u32 = 30;
const HUNKB_FAST: u32 = 31;
const HUNKF_ADVISORY: u32 = 1<<29;
const HUNKF_CHIP: u32 = 1 << 30;
const HUNKF_FAST: u32 = 1 << 31;

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

pub struct AmigaHunkParser;

impl AmigaHunkParser {
    pub fn parse_file(filename: &str) -> io::Result<()> {
        let mut index = 0;

        let mut file = try!(File::open(filename));

        let hunk_header = try!(file.read_u32::<BigEndian>());
        if hunk_header != HUNK_HEADER  {
            println!("Unable to find correct HUNK_HEADER in {} found {} but expected {}", filename, HUNK_HEADER, hunk_header);
            return Err(Error::new(ErrorKind::Other, "oh no!"));
        };

        while try!(file.read_u32::<BigEndian>()) != 0 {

        }

        //println!("b {}", hunk_header);
        Ok(())
    }
}
