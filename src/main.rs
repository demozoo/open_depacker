use clap::Parser;

/// Open Depacker
#[derive(Parser, Debug)]
#[clap(author, version, about, long_about = None)]
struct Args {
    /// File to scan
    #[clap(short, long)]
    scan: String,
}

fn main() {}

/*
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

let amiga = Amiga::new();

if let Some(ref file) = matches.value_of("input") {
    amiga.load_executable_to_memory(file).unwrap();
}
*/
