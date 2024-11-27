// import config.rs
mod config;
mod rabbit;
use log::{error, info};

#[tokio::main]
async fn main() {
    // initialize logger
    env_logger::init();

    // load and validate dotenv
    let status = config::load_env();
    if !status {
        error!("Failed to load config file. Check your logs");
        std::process::exit(1);
    }
    info!("Config file loaded successfully.");

    let res = rabbit::build().await;
    if res.is_err() {
        error!("Failed to initialize RabbitMQ handler. Check your logs");
        std::process::exit(1);
    }

    // get rabbit handler
    let rabbit = res.unwrap();

    // start consuming messages
    let res = rabbit.consume().await;
    if res.is_err() {
        error!("Failed to consume messages. Check your logs");
    }

    // close rabbit connection
    rabbit.close().await;
}
