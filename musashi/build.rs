extern crate gcc;

fn main() {
    let mut cfg = gcc::Config::new();

    cfg.include("../external/musashi");
    cfg.include("../external/musashi/generated");

    let files = &[
        "../external/musashi/m68kcpu.c",
        "../external/musashi/m68kdasm.c",
        "../external/musashi/generated/m68kopac.c",
        "../external/musashi/generated/m68kopdm.c",
        "../external/musashi/generated/m68kopnz.c",
        "../external/musashi/generated/m68kops.c",
    ];

    for f in files.iter() {
        cfg.file(*f);
    }

    cfg.compile("libmusashi.a");
}
