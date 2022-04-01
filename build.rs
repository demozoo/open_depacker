use walkdir::WalkDir;

fn get_files(path: &str) -> Vec<String> {
    let files: Vec<String> = WalkDir::new(path)
        .into_iter()
        .filter_map(|e| {
            let file = e.unwrap();
            let metadata = file.metadata().unwrap();

            if let Some(filename) = file.path().to_str() {
                if metadata.is_file() && !filename.ends_with(".hpp") && filename != ".clang-format"
                {
                    return Some(file);
                }
            }

            None
        })
        .map(|entry| entry.into_path().to_str().unwrap().to_string())
        .collect();
    files
}

fn add_files(build: &mut cc::Build, path: &str) {
    let files = get_files(path);
    build.files(files);
}

fn main() {
    let mut build = cc::Build::new();
    //let env = std::env::var("TARGET").unwrap();

    println!("cargo:rerun-if-changed=external/lzx");

    build.file("external/lzx/crc32.c");
    build.file("external/lzx/lzx_lzx.c");
    build.file("external/lzx/lzx_unpack.c");

    //add_files(&mut build, "external/lzx");

    build.compile("liblzx.a");

    /*
    println!("cargo:rerun-if-changed=external/ancient");

    if env.contains("windows") {
        build.flag("/std:c++latest");
    } else if env.contains("darwin") {
        build.flag("-std=c++17");
    } else {
        build.flag("-std=c++17");
        build.flag("-Wno-unused-parameter");
        build.cpp_link_stdlib("stdc++");
    }

    build.include("external/ancient/api");
    build.include("external/ancient/api/ancient");
    build.include("external/ancient/src");

    add_files(&mut build, "external/ancient/src");

    build.compile("libancient.a");

    // linker stuff
    if env.contains("windows") {
        println!("cargo:rustc-link-lib=Rpcrt4");
    } else if env.contains("darwin") {
        println!("cargo:rustc-link-lib=c++");
    } else {
        println!("cargo:rustc-link-lib=stdc++");
    }
    */
}
