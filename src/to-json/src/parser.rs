use serde_json as json;
use serde_yaml as yaml;

#[derive(Debug)]
pub enum Value {
    JSON(json::Value),
    YAML(yaml::Value),
    TOML(toml::Value),
}

fn json_parser(content: &str) -> Option<json::Value> { // parse json content
    match json::from_str::<json::Value>(content) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

fn yaml_parser(content: &str) -> Option<yaml::Value> { // parse yaml content
    match yaml::from_str::<yaml::Value>(content) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

fn toml_parser(content: &str) -> Option<toml::Value> { // parse toml content
    match toml::from_str::<toml::Value>(content) {
        Ok(result) => Some(result),
        Err(_) => None,
    }
}

pub fn parser(content: &str) -> Result<Value, String> {
    match json_parser(content) { // try JSON format
        Some(data) => Ok(Value::JSON(data)),
        None => match toml_parser(content) { // try TOML format
            Some(data) => Ok(Value::TOML(data)),
            None => match yaml_parser(content) { // try YAML format
                Some(data) => Ok(Value::YAML(data)),
                None => Err(String::from("unknown input format")),
            }
        }
    }
}
