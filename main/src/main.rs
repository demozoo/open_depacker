#[macro_use]
extern crate clap;
extern crate byteorder;

pub mod amiga_hunk_parser;
use amiga_hunk_parser::HunkParser;

fn main() {
    let matches = clap_app!(open_depacker =>
        (version: "0.1")
        (author: "Daniel Collin. <daniel@collin.com>")
        (about: "Generic data depacker")
        (@arg input: +required "Sets the input file to use")
        (@arg debug: -d ... "Sets the level of debugging information")
    ).get_matches();

    match matches.occurrences_of("debug") {
        0 => println!("Debug mode is off"),
        1 => println!("Debug mode is kind of on"),
        2 => println!("Debug mode is on"),
        3 | _ => println!("Don't be crazy"),
    }

    if let Some(ref file) = matches.value_of("input") {
        println!("Using config file: {}", file);
        HunkParser::parse_file(file).unwrap();
    }
}
