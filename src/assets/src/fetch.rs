use std::fs::File;
use std::io::Read;
use std::time::Duration;
use log::{debug, info, warn};
use std::collections::HashSet;
use reqwest_middleware::ClientBuilder;
use reqwest_retry::RetryTransientMiddleware;
use reqwest_retry::policies::ExponentialBackoff;

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
    let retry_policy = ExponentialBackoff::builder()
        .backoff_exponent(2) // [2, 4, 8, 16, ...]
        .retry_bounds(
            Duration::from_secs(5), // min retry interval -> 1s
            Duration::from_secs(60)) // max retry interval -> 60s
        .build_with_max_retries(2); // total request 3 times
    let client = ClientBuilder::new(reqwest::Client::new())
        .with(RetryTransientMiddleware::new_with_policy(retry_policy))
        .build();
    debug!("Start downloading `{}`", url);
    match client.get(url)
        .timeout(Duration::from_secs(timeout))
        .send().await {
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

#[cfg(test)]
mod tests {
    use std::fs;
    use std::pin::Pin;
    use std::future::Future;
    use std::fs::OpenOptions;
    use std::io::Write;
    use super::{asset_tidy, remove_dup};
    use super::{http_fetch, local_fetch, asset_fetch};

    const TEST_DATA: &str = "\tabc  \n123 \n 456\r\nabc\n\n789  ";

    #[test]
    fn basic() {
        assert_eq!(asset_tidy(TEST_DATA), vec![
            String::from("abc"),
            String::from("123"),
            String::from("456"),
            String::from("abc"),
            String::from("789"),
        ]);
        assert_eq!(remove_dup(&asset_tidy(TEST_DATA)), vec![
            String::from("abc"),
            String::from("123"),
            String::from("456"),
            String::from("789"),
        ]);
    }

    fn run_async<T>(func: Pin<Box<impl Future<Output=T>>>) {
        tokio::runtime::Builder::new_multi_thread()
            .enable_all()
            .build()
            .unwrap()
            .block_on(func);
    }

    fn gen_test_file() {
        let mut fp = OpenOptions::new()
            .write(true)
            .create(true)
            .truncate(true)
            .open("/tmp/assets_test_file")
            .unwrap();
        fp.write_all(TEST_DATA.as_ref()).expect("test file create error");
    }

    #[test]
    fn asset() {
        // test http_fetch function
        run_async(Box::pin(async {
            assert!(http_fetch("invalid url", 10).await.is_err());
            assert_eq!(
                http_fetch("https://gstatic.com/generate_204", 10).await,
                Ok(vec![])
            );
        }));

        // test local_fetch function
        gen_test_file();
        run_async(Box::pin(async {
            assert!(local_fetch("/").await.is_err());
            assert_eq!(
                local_fetch("/tmp/assets_test_file").await,
                Ok(vec![
                    String::from("abc"),
                    String::from("123"),
                    String::from("456"),
                    String::from("abc"),
                    String::from("789"),
                ])
            );
        }));

        // test combine asset_fetch function
        run_async(Box::pin(async {
            assert!(
                asset_fetch("", &vec![]).await.unwrap().is_empty()
            );
            assert!(
                asset_fetch("", &vec![String::from("...")]).await.is_none()
            );
            assert_eq!(
                asset_fetch("", &vec![
                    String::from("/tmp/assets_test_file"),
                    String::from("https://gstatic.com/generate_204")
                ]).await,
                Some(vec![
                    String::from("abc"),
                    String::from("123"),
                    String::from("456"),
                    String::from("789"),
                ])
            );
        }));
        fs::remove_file("/tmp/assets_test_file").expect("test file delete error");
    }
}
