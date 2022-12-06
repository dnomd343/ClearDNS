use crate::parser::{parser, Value};

fn json_convert(content: &str) -> Result<String, String> { // convert to JSON format
    let data = match parser(content)? {
        Value::JSON(json) => serde_json::to_string(&json),
        Value::YAML(yaml) => serde_json::to_string(&yaml),
        Value::TOML(toml) => serde_json::to_string(&toml),
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
