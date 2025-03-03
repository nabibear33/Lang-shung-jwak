use std::env;
use std::fs::File;
use std::io::Read;
use jwak_rust::*;


fn main() -> Result<(), Error> {
    let args: Vec<String>= env::args().collect();

    if args.len() != 2 {
        eprintln!("How to use: cargo run <filename.jwak>");
        return Err(Error::SyntaxError);
    }
    let filename = &args[1];
    let mut file = File::open(filename).unwrap();
    let mut contents = String::new();
    let _ = file.read_to_string(&mut contents);
    
    let mut interpreter = LangShungJwak::new();
    let _ = interpreter.compile(&contents);
    Ok(())
}