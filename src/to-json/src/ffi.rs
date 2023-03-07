use std::os::raw::c_char;
use std::ffi::{CStr, CString};
use crate::parser::{parser, Value};

/// Load c-style string from `const char *` pointer.
#[inline]
unsafe fn load_c_string(ptr: *const c_char) -> String {
    CString::from(CStr::from_ptr(ptr))
        .into_string()
        .unwrap()
}

/// Export c-style string as `const char *` pointer.
/// # NOTE
/// The exported string cannot be freed by the c language `free(void *)` function,
/// but should use the `free_rust_string` callback function, if this interface is
/// not called, a memory leak will occur.
#[inline]
fn export_c_string(string: String) -> *const c_char {
    CString::new(string).unwrap().into_raw()
}

/// Free the exported c-style string.
#[no_mangle]
pub unsafe extern "C" fn free_rust_string(ptr: *const c_char) {
    let _ = CString::from_raw(ptr as *mut _); // reclaim rust ownership
}

/// Deserialize text content and serialize to JSON format.
fn json_format(content: &str) -> Option<String> {
    let result = match parser(&content) {
        Ok(value) => match value {
            Value::JSON(json) => serde_json::to_string(&json),
            Value::YAML(yaml) => serde_json::to_string(&yaml),
            Value::TOML(toml) => serde_json::to_string(&toml),
        },
        _ => return None,
    };
    match result {
        Ok(data) => Some(data),
        Err(_) => None,
    }
}

/// Format the input text into JSON format and return a c-style string, or return
/// `NULL` if an error occurs.
#[no_mangle]
pub unsafe extern "C" fn to_json(content: *const c_char) -> *const c_char {
    let content = load_c_string(content);
    match json_format(&content) {
        Some(data) => export_c_string(data),
        None => std::ptr::null(),
    }
}

#[cfg(test)]
mod tests {
    use super::json_format;

    const JSON_TEST_STR: &str = r#"
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

    const YAML_TEST_STR: &str = r#"
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

    const TOML_TEST_STR: &str = r#"
int = 123
bool = true
float = 3.141592
string = "json test"
array = [ 1, 2, 3, 4, 5 ]

[object]
sub = "test"
"#;

    #[inline]
    fn format(raw: &str) -> String {
        match json_format(raw) {
            Some(data) => data,
            None => panic!("format error"),
        }
    }

    #[test]
    fn json_input() {
        assert_eq!(
            format(JSON_TEST_STR),
            format(&json_format(JSON_TEST_STR).unwrap()),
        );
    }

    #[test]
    fn yaml_input() {
        assert_eq!(
            format(JSON_TEST_STR),
            format(&json_format(YAML_TEST_STR).unwrap()),
        );
    }

    #[test]
    fn toml_input() {
        assert_eq!(
            format(JSON_TEST_STR),
            format(&json_format(TOML_TEST_STR).unwrap()),
        );
    }
}
