use std::io::Write;
use std::env::set_var;
use std::path::PathBuf;
use std::fs::OpenOptions;
use std::os::raw::c_char;
use log::{debug, trace, warn};
use std::ffi::{CStr, CString};
use crate::fetch::asset_fetch;

/// Compatible with C89 bool value.
const TRUE: u8 = 1;
const FALSE: u8 = 0;

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

/// Initialize the rust module log, enable trace level log when verbose is not `0`.
#[no_mangle]
pub unsafe extern "C" fn assets_log_init(verbose: u8) {
    if verbose == FALSE { // bool value `FALSE`
        set_var("RUST_LOG", "info");
    } else {
        set_var("RUST_LOG", "trace");
    }
    env_logger::init();
}

/// Update the specified resource file, return `0` on failure.
#[no_mangle]
#[tokio::main]
pub async unsafe extern "C" fn asset_update(
    file: *const c_char, sources: *const *const c_char, assets_dir: *const c_char) -> u8 {

    let name = load_c_string(file); // import c-style string
    let sources = load_c_string_list(sources);
    let assets_dir = load_c_string(assets_dir);
    trace!("Working folder is `{}`", assets_dir);
    trace!("Updating `{}` from {:?}", name, sources);

    let assets_dir = PathBuf::from(&assets_dir);
    let is_remote = |src: &str| {
        src.starts_with("http://") || src.starts_with("https://")
    };
    let sources = sources.iter()
        .map(|src| {
            if !is_remote(&src) && !src.starts_with("/") { // local relative path
                let file_path = assets_dir.join(src);
                String::from(file_path.to_str().unwrap())
            } else {
                src.clone()
            }
        })
        .collect::<Vec<String>>();
    let file = String::from(assets_dir.join(&name).to_str().unwrap());
    debug!("Asset sources -> {:?}", sources);
    debug!("Asset target -> `{}`", file);

    match asset_fetch(&name, &sources).await {
        Some(data) => {
            let mut content = String::new();
            let _ = data.iter().map(|item| {
                content.push_str(item);
                content.push('\n');
            }).collect::<Vec<()>>();
            match OpenOptions::new()
                .write(true)
                .create(true)
                .truncate(true)
                .open(&file) { // open target file
                Ok(mut fp) => {
                    match fp.write_all(content.as_ref()) {
                        Err(err) => warn!("File `{}` save error: {}", file, err),
                        _ => debug!("File `{}` save success", file),
                    }
                },
                Err(err) => warn!("File `{}` open failed: {}", file, err),
            };
            TRUE // asset fetch success
        },
        None => FALSE, // asset fetch failed
    }
}
