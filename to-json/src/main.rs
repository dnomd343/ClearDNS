use serde_json::{Value};

fn json_str() -> String {
    let json = r#"{
  "demo": "key_1",
  "author": "dnomd343",
  "test": [
    "123",
    "234",
    "345"
  ]
}"#;
    return String::from(json);
}

fn main() {
    let raw = json_str();
    println!("JSON raw content ->\n{}", raw);

    let parsed: Value = serde_json::from_str(&raw[..]).unwrap();
    println!("Author -> {}", parsed["author"]);
}
