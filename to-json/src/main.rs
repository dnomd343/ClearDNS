use std::io::Read;
use serde_json as json;
use serde_yaml as yaml;

const FILE_ERROR: i32 = 1; // file open error
const PARSE_ERROR: i32 = 2; // parser error

enum Format {
    JSON(json::Value),
    YAML(yaml::Value),
    TOML(toml::Value),
}

fn parser(content: &str) -> Option<Format> {
    if let Ok(data) = json::from_str::<json::Value>(content) { // try JSON format
        return Some(Format::JSON(data));
    }
    if let Ok(data) = toml::from_str::<toml::Value>(content) { // try TOML format
        return Some(Format::TOML(data));
    }
    if let Ok(data) = yaml::from_str::<yaml::Value>(content) { // try YAML format
        return Some(Format::YAML(data));
    }
    return None; // parse failed
}

fn to_json(content: &str) -> String { // convert to JSON format
    match parser(content) {
        Some(data) => match data {
            Format::JSON(dat) => json::to_string(&dat).unwrap(),
            Format::YAML(dat) => json::to_string(&dat).unwrap(),
            Format::TOML(dat) => json::to_string(&dat).unwrap(),
        },
        None => std::process::exit(PARSE_ERROR), // error exit
    }
}

fn read_file(path: &str) -> String { // read file content
    match std::fs::File::open(path) {
        Ok(mut file) => { // file open success
            let mut content = String::new();
            file.read_to_string(&mut content).unwrap(); // get file content
            content
        },
        Err(_) => std::process::exit(FILE_ERROR), // read failed
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect(); // input arguments
    if args.len() < 2 { // missing arguments
        println!("usage: toJSON [file]");
        std::process::exit(0);
    }
    let content = read_file(&args[1].clone()[..]); // read file content
    println!("{}", to_json(&content[..])); // convert to JSON format
}
