use reqwest::Client;
use std::env::set_var;
use std::time::Duration;

const TIMEOUT: u64 = 60;

async fn asset_fetch(url: &str) -> Result<String, String> {
    let client = Client::builder()
        .timeout(Duration::from_secs(TIMEOUT))
        .build().unwrap();
    match client.get(url).send().await {
        Ok(response) => {
            match response.text().await {
                Ok(text) => {
                    Ok(text)
                },
                Err(err) => Err(format!("http content error: {}", err))
            }
        },
        Err(err) => Err(format!("http request failed: {}", err))
    }
}

async fn demo() {

    println!("demo function start");

    match asset_fetch("https://res.343.re/Share/cleardns/gfwlist.txt").await {
        Ok(data) => {
            // println!("{}", data);
        },
        Err(err) => println!("error -> {}", err)
    }


    println!("demo function exit");

}

#[tokio::main]
async fn main() {

    // set_var("RUST_LOG", "debug");
    set_var("RUST_LOG", "trace");
    env_logger::init();

    demo().await;

    println!("end demo");

}
