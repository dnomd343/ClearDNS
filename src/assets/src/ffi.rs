use std::ffi::{CStr, CString};
use std::os::raw::c_char;

/// Load c-style string from `char *` pointer.
unsafe fn load_c_string(ptr: *const c_char) -> String {

    println!("c-style ptr:     {:?}", ptr);

    // let s = CStr::from_ptr(ptr).to_str().unwrap();
    //
    // println!("rust cstr ptr:   {:?}", s.as_ptr());
    //
    // let string = format!("{}", s);
    //
    // println!("rust string ptr: {:?}", string.as_ptr());
    //
    // println!();


    let a = CString::from(CStr::from_ptr(ptr));
    println!("rust string ptr: {:?}", a.as_ptr());

    a.into_string().unwrap()

    // String::from(CStr::from_ptr(ptr).to_str().unwrap())
}

/// Load c-style string list from `char **` pointer.
unsafe fn load_c_string_list(ptr: *const *const c_char) -> Vec<String> {
    let mut index = 0;
    let mut string_list: Vec<String> = vec![];
    while *ptr.offset(index) != std::ptr::null() { // traverse until `NULL`
        let s = load_c_string(*ptr.offset(index));

        // println!("string ptr: `{:?}`", s.as_ptr());
        println!("content: `{}`", s);

        println!();

        string_list.push(s);

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
