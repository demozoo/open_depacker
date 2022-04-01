fn main() {
    let mut cfg = cc::Build::new();

    cfg.include("external/lzx");

    let files = &[
        "external/lzx/crc32.c",
        "external/lzx/lzx_lzx.c",
        "external/lzx/lzx_unpack.c",
        "external/lzx/lzx_wrap.c",
    ];

    for f in files.iter() {
        cfg.file(*f);
    }

    cfg.compile("liblzx.a");
}
