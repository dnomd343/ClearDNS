use serde_json as json;
use crate::parser::{parser, Value};

fn json_convert(content: &str) -> Result<String, String> { // convert to JSON format
    let data = match parser(content)? {
        Value::JSON(_json) => json::to_string(&_json),
        Value::YAML(_yaml) => json::to_string(&_yaml),
        Value::TOML(_toml) => json::to_string(&_toml),
    };
    match data {
        Ok(data) => Ok(data),
        Err(err) => Err(err.to_string()),
    }
}

pub fn to_json(content: &str) -> Option<String> { // to JSON string
    match json_convert(content) {
        Ok(data) => Some(data),
        Err(_) => None,
    }
}
