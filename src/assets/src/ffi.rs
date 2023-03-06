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
pub unsafe extern "C" fn rust_test_single(ptr: *const c_char) {
    println!("enter rust function");

    let ret = load_c_string(ptr);
    println!("ret: {:?}", ret);

    println!("exit rust function");
}


#[no_mangle]
pub unsafe extern "C" fn rust_test_multi(ptr: *const *const c_char) {
    println!("enter rust function");

    let ret = load_c_string_list(ptr);
    println!("ret: {:?}", ret);

    println!("exit rust function");
}
