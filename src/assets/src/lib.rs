// mod fetch;
//
// use std::env::set_var;
//
// use crate::fetch::Asset;

use std::ffi::{c_char, CStr};

// const TIMEOUT: u64 = 60;
//
// const ASSETS_DIR: &str = "/cleardns/assets/";

#[no_mangle]
pub unsafe extern "C" fn rust_test(ptr: *const *const c_char) {
    println!("enter rust function");

    unsafe {

        println!("ptr: {:?}", ptr);


        let mut index = 0;
        while *ptr.offset(index) != std::ptr::null() {

            let p = *ptr.offset(index);

            println!("loop: {:?}", p);

            let s = String::from(
                CStr::from_ptr(p).to_str().unwrap()
            );

            println!("content: {}", s);

            index += 1;
        }


    }

    println!("exit rust function");
}

// #[tokio::main]
// async fn main() {
//
//     set_var("RUST_LOG", "trace");
//     env_logger::init();
//
//     let d = vec![
//         String::from("https://res.343.re/Share/cleardns/gfwlist.txt"),
//         String::from("/tmp/gfwlist.txt"),
//     ];
//     let info = Asset {
//         name: String::from("demo"),
//         timeout: TIMEOUT,
//         workdir: String::from(ASSETS_DIR),
//         sources: d,
//     };
//     fetch::asset_fetch(&info).await;
//
//
//     println!("end demo");
//
// }
