use crate::convert::to_json;
use std::ffi::{c_char, CStr, CString};

fn to_c_string(string: String) -> *const c_char { // fetch c-style ptr of string
    CString::new(string).unwrap().into_raw()
}

unsafe fn load_c_string(ptr: *const c_char) -> String { // load string from c-style ptr
    String::from(
        CStr::from_ptr(ptr).to_str().unwrap()
    )
}

#[no_mangle]
pub unsafe extern "C" fn free_rust_string(ptr: *const c_char) { // free string memory
    let _ = CString::from_raw(ptr as *mut _);
}

#[no_mangle]
pub unsafe extern "C" fn to_json_ffi(content: *const c_char) -> *const c_char {
    let content = load_c_string(content);
    let result = match to_json(&content) { // convert to JSON format
        Some(data) => data,
        None => String::new(), // convert failed -> empty string
    };
    to_c_string(result) // return c-style ptr
}
