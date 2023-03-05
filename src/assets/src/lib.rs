// mod fetch;
//
// use std::env::set_var;
//
// use crate::fetch::Asset;

mod ffi;

// use std::ffi::{c_char, CStr};

// const TIMEOUT: u64 = 60;
//
// const ASSETS_DIR: &str = "/cleardns/assets/";


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
