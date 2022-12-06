const JSON_TEST_CONTENT: &str = r#"
{
  "int": 123,
  "float": 2.3333,
  "string": "dnomd343",
  "array": [1, 2, 3, 4, 5],
  "dict": {
    "test": "demo"
  }
}
"#;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn json_to_json() {
        println!("{}", JSON_TEST_CONTENT);
        assert_eq!(1, 2);
    }

}
