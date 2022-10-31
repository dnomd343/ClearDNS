use serde_json::Value as JsonValue;
use serde_yaml::Value as YamlValue;
use toml::Value as TomlValue;

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

fn main() {
    println!("---------------------------------");
    println!("JSON raw content:\n{}", JSON_STR);
    println!("---------------------------------");
    println!("YAML raw content:\n{}", YAML_STR);
    println!("---------------------------------");
    println!("TOML raw content:\n{}", TOML_STR);
    println!("---------------------------------");

    // let data: JsonValue = serde_json::from_str(JSON_STR).unwrap();
    // println!("{:#?}", data);
    // let data: YamlValue = serde_yaml::from_str(YAML_STR).unwrap();
    // println!("{:#?}", data);
    let data: TomlValue = toml::from_str(TOML_STR).unwrap();
    println!("{:#?}", data);

    let ret = serde_json::to_string(&data).unwrap();
    println!("JSON output ->\n{}", ret);
}
