use serde_json as json;
use serde_yaml as yaml;

const JSON_STR: &str = r#"
{
  "demo": "key_1",
  "author": "dnomd343",
  "test": [
    "123",
    "234",
    "345"
  ]
}
"#;

const YAML_STR: &str = r#"
demo: key_1
author: dnomd343
test:
  - 123
  - 234
  - 345
"#;

const TOML_STR: &str = r#"
demo = "key_1"
author = "dnomd343"
test = [ 123, 234, 345 ]
"#;

#[derive(Debug)]
enum Format {
    JSON(json::Value),
    YAML(yaml::Value),
    TOML(toml::Value),
}

fn parser(raw: &str) -> Option<Format> {
    if let Ok(data) = json::from_str::<json::Value>(raw) { // try JSON format
        return Some(Format::JSON(data));
    }
    if let Ok(data) = toml::from_str::<toml::Value>(raw) { // try TOML format
        return Some(Format::TOML(data));
    }
    if let Ok(data) = yaml::from_str::<yaml::Value>(raw) { // try YAML format
        return Some(Format::YAML(data));
    }
    return None;
}

fn to_json(raw: &str) -> String {
    match parser(raw) {
        Some(data) => match data {
            Format::JSON(dat) => json::to_string(&dat).unwrap(),
            Format::YAML(dat) => json::to_string(&dat).unwrap(),
            Format::TOML(dat) => json::to_string(&dat).unwrap(),
        },
        None => std::process::exit(1),
    }
}

fn main() {
    println!("---------------------------------");
    println!("JSON raw content:\n{}", JSON_STR);
    println!("---------------------------------");
    println!("YAML raw content:\n{}", YAML_STR);
    println!("---------------------------------");
    println!("TOML raw content:\n{}", TOML_STR);
    println!("---------------------------------");

    let temp = to_json(JSON_STR);
    println!("{}", temp);

    // let a = String::from("ok");
    // match parser(JSON_STR) {
    //     Some(data) => {
    //         // println!("{:#?}", data);
    //         match data {
    //             Format::JSON(dat) => {
    //                 return json::to_string(&data).unwrap()
    //             },
    //             _ => println!("123"),
    //             // Format::JSON(dat) => println!("{:#?}", dat),
    //             // Format::YAML(dat) => println!("{:#?}", dat),
    //             // Format::TOML(dat) => println!("{:#?}", dat),
    //         }
    //         // let temp = data;
    //         // let ret = json::to_string(&data).unwrap();
    //         // println!("JSON output ->\n{}", ret);
    //     },
    //     None => println!("parser error"),
    // }

    // let data = parser(JSON_STR);
    // println!("{:#?}", data);

    // decode(YAML_STR);
    // decode(TOML_STR);



    // let ret = serde_json::to_string(&data).unwrap();
    // println!("JSON output ->\n{}", ret);
}
