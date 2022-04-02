use clap::Parser;
use core::slice;
// use rayon::prelude::*;
use sha2::Digest;
use std::collections::HashSet;
use std::ffi::CStr;
use std::fs;
use std::fs::File;
use std::io::prelude::*;
use std::path::Path;
use walkdir::WalkDir;

/// Open Depacker
#[derive(Parser, Debug)]
#[clap(author, version, about, long_about = None)]
struct Args {
    /// File(s) to scan
    #[clap(short, long)]
    scan: String,

    /// Do recurseive scanning (include sub-directories) when using --scan
    #[clap(short, long)]
    recursive: bool,
}

#[repr(C)]
struct CFileEntry {
    filename: *const u8,
    data: *const u8,
    data_len: u32,
}

#[repr(C)]
struct CFilelist {
    entries: *const CFileEntry,
    count: u32,
    cap: u32,
}

extern "C" {
    fn lzx_unpack_wrap(buffer: *mut u8, len: i32) -> CFilelist;
    fn lzx_free_entries(list: CFilelist);
}

// Get files for a given directory
fn get_files(path: &str, recurse: bool) -> Vec<String> {
    if !Path::new(path).exists() {
        println!(
            "Path/File \"{}\" doesn't exist. No file(s) will be processed.",
            path
        );
        return Vec::new();
    }

    // Check if "path" is a single file
    let md = std::fs::metadata(path).unwrap();

    if md.is_file() {
        return vec![path.to_owned()];
    }

    let max_depth = if !recurse { 1 } else { usize::MAX };

    let files: Vec<String> = WalkDir::new(path)
        .max_depth(max_depth)
        .into_iter()
        .filter_map(|e| {
            let file = e.unwrap();
            let metadata = file.metadata().unwrap();

            if let Some(filename) = file.path().to_str() {
                if metadata.is_file() && !filename.ends_with(".listing") {
                    return Some(filename.to_owned());
                }
            }
            None
        })
        .collect();
    files
}

unsafe fn process_file(filename: &str) {
    let mut data = fs::read(filename).unwrap();

    println!("Scaning file {}", filename);
    let input_file = std::path::Path::new(&filename);
    let input_file_dir = input_file.parent().unwrap();
    let mut dir_entry = 0;
    let mut written_data = HashSet::new();

    for i in 0..data.len() - 3 {
        let t = &mut data[i..];
        let list = lzx_unpack_wrap(t.as_mut_ptr(), t.len() as _);

        if list.count > 0 {
            let entries = slice::from_raw_parts(list.entries, list.count as _);
            let mut did_write_files = false;

            for e in entries {
                let filename = CStr::from_ptr(e.filename as _);
                let p = filename.to_string_lossy().into_owned();

                let data = slice::from_raw_parts(e.data, e.data_len as _);
                let hash = sha2::Sha256::digest(&data);
                let hash_str = format!("{:x}", hash);

                if written_data.contains(&hash_str) {
                    continue;
                } else {
                    written_data.insert(hash_str);
                    did_write_files = true;
                }

                let path = std::path::Path::new(&p);
                let full_path = input_file_dir.join(&format!("{}/", dir_entry)).join(path);
                let prefix = full_path.parent().unwrap();
                std::fs::create_dir_all(prefix).unwrap();
                println!("Writing file {:?}", full_path);
                let mut f = File::create(full_path).unwrap();
                f.write_all(data).unwrap();
            }

            if did_write_files {
                dir_entry += 1;
            }

            lzx_free_entries(list);
        }
    }
}

fn main() {
    let args = Args::parse();
    let files = get_files(&args.scan, args.recursive);

    for file in files {
        unsafe { process_file(&file) };
    }
}
