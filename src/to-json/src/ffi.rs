use std::ffi::{c_char, CStr, CString};

#[no_mangle]
pub unsafe extern "C" fn free_rust_string(string: *const c_char) {
    let _ = CString::from_raw(string as *mut _);
}

#[no_mangle]
pub unsafe extern "C" fn to_json_rust(content: *const c_char) -> *const c_char {
    let content: &str = CStr::from_ptr(content).to_str().unwrap();

    // TODO: remove test code
    println!("Raw content -> {}", content);

    // TODO: convert to JSON format

    CString::new(content).unwrap().into_raw()
}
