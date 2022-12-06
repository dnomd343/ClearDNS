use crate::convert::to_json;

const JSON_TEST_CONTENT: &str = r#"
{
  "int": 123,
  "bool": true,
  "float": 3.141592,
  "string": "json test",
  "array": [1, 2, 3, 4, 5],
  "object": {
    "sub": "test"
  }
}
"#;

const YAML_TEST_CONTENT: &str = r#"
int: 123
bool: true
float: 3.141592
string: "json test"
array:
 - 1
 - 2
 - 3
 - 4
 - 5
object:
  sub: test
"#;

const TOML_TEST_CONTENT: &str = r#"
int = 123
bool = true
float = 3.141592
string = "json test"
array = [ 1, 2, 3, 4, 5 ]

[object]
sub = "test"
"#;

fn format_json(raw: &str) -> String {
    match to_json(raw) {
        Some(data) => data,
        None => panic!("JSON format error"),
    }
}

#[test]
fn json_to_json() {
    assert_eq!(
        format_json(JSON_TEST_CONTENT),
        format_json(&to_json(JSON_TEST_CONTENT).unwrap()),
    );
}

#[test]
fn yaml_to_json() {
    assert_eq!(
        format_json(JSON_TEST_CONTENT),
        format_json(&to_json(YAML_TEST_CONTENT).unwrap()),
    );
}

#[test]
fn toml_to_json() {
    assert_eq!(
        format_json(JSON_TEST_CONTENT),
        format_json(&to_json(TOML_TEST_CONTENT).unwrap()),
    );
}
