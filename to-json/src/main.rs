use serde_json::{Value};

const JSON_STR: &str = r#"{
  "demo": "key_1",
  "author": "dnomd343",
  "test": [
    "123",
    "234",
    "345"
  ]
}"#;

fn main() {
    println!("JSON raw content ->\n{}", JSON_STR);

    let data: Value = serde_json::from_str(JSON_STR).unwrap();
    println!("{:#?}", data);

    let ret = serde_json::to_string(&data).unwrap();
    println!("JSON output ->\n{}", ret);
}
