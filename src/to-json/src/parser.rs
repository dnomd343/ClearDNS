use serde_json as json;
use serde_yaml as yaml;

#[derive(Debug)]
pub enum Value {
    JSON(json::Value),
    YAML(yaml::Value),
    TOML(toml::Value),
}

/// Deserialize text content into JSON format.
fn json_parser(content: &str) -> Option<json::Value> {
    match json::from_str::<json::Value>(content) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

/// Deserialize text content into YAML format.
fn yaml_parser(content: &str) -> Option<yaml::Value> {
    match yaml::from_str::<yaml::Value>(content) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

/// Deserialize text content into TOML format.
fn toml_parser(content: &str) -> Option<toml::Value> {
    match toml::from_str::<toml::Value>(content) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

/// Try to deserialize the text in JSON, TOML and YAML format.
pub fn parser(content: &str) -> Result<Value, &'static str> {
    match json_parser(content) { // try JSON format
        Some(data) => Ok(Value::JSON(data)),
        None => match toml_parser(content) { // try TOML format
            Some(data) => Ok(Value::TOML(data)),
            None => match yaml_parser(content) { // try YAML format
                Some(data) => Ok(Value::YAML(data)),
                None => Err("unknown input format"),
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::Value;
    use super::parser;
    use super::json_parser;
    use super::yaml_parser;
    use super::toml_parser;

    const JSON_STR: &str = "{\"test\": \"ok\"}";
    const YAML_STR: &str = "test: ok";
    const TOML_STR: &str = "test = \"ok\"";

    #[test]
    fn json() {
        assert!(json_parser("").is_none()); // parse invalid text
        assert!(json_parser(JSON_STR).is_some());
    }

    #[test]
    fn yaml() {
        assert!(yaml_parser("").is_none()); // parse invalid text
        assert!(yaml_parser(YAML_STR).is_some());
    }

    #[test]
    fn toml() {
        assert!(toml_parser(".").is_none()); // parse invalid text
        assert!(toml_parser(TOML_STR).is_some());
    }

    #[test]
    fn global() {
        match parser(JSON_STR).unwrap() {
            Value::JSON(_) => (),
            _ => panic!("JSON parser error"),
        };
        match parser(YAML_STR).unwrap() {
            Value::YAML(_) => (),
            _ => panic!("YAML parser error"),
        };
        match parser(TOML_STR).unwrap() {
            Value::TOML(_) => (),
            _ => panic!("TOML parser error"),
        };
        assert!(parser("\0").is_err()); // parse invalid text
    }
}
