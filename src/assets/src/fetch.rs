use std::fs::File;
use std::io::Read;
use reqwest::Client;
use std::time::Duration;
use log::{debug, info, warn};
use std::collections::HashSet;

/// Http download timeout limit
const TIMEOUT: u64 = 120;

/// Cut text line by line and remove invisible characters on both sides.
fn asset_tidy(data: &str) -> Vec<String> {
    data.lines()
        .map(|x| String::from(x.trim()))
        .filter(|x| !x.is_empty())
        .collect()
}

/// Remove duplicate elements from an array.
fn remove_dup(data: &Vec<String>) -> Vec<String> {
    let mut result: Vec<String> = vec![];
    let mut tmp: HashSet<String> = HashSet::new();
    for val in data {
        if tmp.insert(val.clone()) { // value already exist
            result.push(val.clone());
        }
    }
    result
}

/// Download the specified text file and organize it into a String array.
async fn http_fetch(url: &str, timeout: u64) -> Result<Vec<String>, String> {
    let client = Client::builder()
        .timeout(Duration::from_secs(timeout))
        .build().unwrap();
    debug!("Start downloading `{}`", url);
    match client.get(url).send().await {
        Ok(response) => {
            match response.text().await {
                Ok(text) => {
                    info!("Remote file `{}` download success", url);
                    Ok(asset_tidy(&text))
                },
                Err(err) => Err(format!("http content error: {}", err))
            }
        },
        Err(err) => Err(format!("http request failed: {}", err))
    }
}

/// Read the specified text file and organize it into a String array.
async fn local_fetch(path: &str) -> Result<Vec<String>, String> {
    match File::open(path) {
        Ok(mut file) => {
            let mut text = String::new();
            if let Err(err) = file.read_to_string(&mut text) {
                return Err(format!("file `{}` read failed: {}", path, err));
            };
            info!("Local file `{}` read success", path);
            Ok(asset_tidy(&text))
        },
        Err(err) => Err(format!("file `{}` open failed: {}", path, err)),
    }
}

/// Get multiple resource data and merge them.
pub(crate) async fn asset_fetch(name: &str, sources: &Vec<String>) -> Option<Vec<String>> {
    let is_remote = |src: &str| {
        src.starts_with("http://") || src.starts_with("https://")
    };
    let mut contents: Vec<Vec<String>> = vec![];
    for source in sources {
        contents.push(match if is_remote(&source) {
            http_fetch(source.trim(), TIMEOUT).await // from remote text file
        } else {
            local_fetch(source.trim()).await // from local text file
        } {
            Ok(data) => {
                debug!("Asset source `{}` fetch success with {} items", source.trim(), data.len());
                data
            },
            Err(err) => {
                warn!("Asset source `{}` fetch failed: {}", source.trim(), err);
                return None; // stop fetch process
            }
        });
    }
    let contents = remove_dup(&contents
        .into_iter()
        .flatten()
        .collect::<Vec<String>>());
    info!("Asset `{}` fetch complete with {} items", name, contents.len());
    Some(contents)
}
