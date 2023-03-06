use log::debug;
use std::env::set_var;
use std::os::raw::c_char;
use std::ffi::{CStr, CString};

/// Load c-style string from `char *` pointer.
unsafe fn load_c_string(ptr: *const c_char) -> String {
    CString::from(CStr::from_ptr(ptr))
        .into_string()
        .unwrap()
}

/// Load c-style string list from `char **` pointer.
unsafe fn load_c_string_list(ptr: *const *const c_char) -> Vec<String> {
    let mut index = 0;
    let mut string_list: Vec<String> = vec![];
    while *ptr.offset(index) != std::ptr::null() { // traverse until `NULL`
        string_list.push(load_c_string(*ptr.offset(index)));
        index += 1;
    }
    string_list
}

#[no_mangle]
pub unsafe extern "C" fn assets_log_init(verbose: u8) {
    if verbose == 0 { // bool value `FALSE`
        set_var("RUST_LOG", "info");
    } else {
        set_var("RUST_LOG", "trace");
    }
    env_logger::init();
}

#[no_mangle]
pub unsafe extern "C" fn rust_assets_update(
    file: *const c_char, sources: *const *const c_char, assets_dir: *const c_char) -> u8 {

    let file = load_c_string(file); // import c-style string
    let sources = load_c_string_list(sources);
    let assets_dir = load_c_string(assets_dir);

    debug!("file: {}", file);
    debug!("source: {:?}", sources);
    debug!("assets dir: {}", assets_dir);

    0
}
