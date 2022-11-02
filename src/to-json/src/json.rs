use serde_json as json;
use serde_yaml as yaml;

enum Format {
    JSON(json::Value),
    YAML(yaml::Value),
    TOML(toml::Value),
}

fn parser(content: &str) -> Option<Format> {
    if let Ok(data) = json::from_str::<json::Value>(content) { // try JSON format
        return Some(Format::JSON(data));
    }
    if let Ok(data) = toml::from_str::<toml::Value>(content) { // try TOML format
        return Some(Format::TOML(data));
    }
    if let Ok(data) = yaml::from_str::<yaml::Value>(content) { // try YAML format
        return Some(Format::YAML(data));
    }
    return None; // parse failed
}

pub fn to_json(content: &str) -> String { // convert to JSON format
    match parser(content) {
        Some(data) => match data {
            Format::JSON(dat) => json::to_string(&dat).unwrap(),
            Format::YAML(dat) => json::to_string(&dat).unwrap(),
            Format::TOML(dat) => json::to_string(&dat).unwrap(),
        },
        None => String::from(""), // failed -> empty string
    }
}
