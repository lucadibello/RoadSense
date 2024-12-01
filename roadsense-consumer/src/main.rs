// import config.rs
mod config;
mod rabbit;

use crate::config::load_env;

use log::{debug, error, info};
use std::process::exit;

#[tokio::main]
async fn main() {
    // load and validate dotenv
    let status = load_env();
    if !status {
        println!("Failed to load .env file");
        exit(1);
    }
    info!("Config file loaded successfully.");

    // initialize logger
    env_logger::init();
    debug!("Logger initialized");

    let res = rabbit::build().await;
    if res.is_err() {
        error!("Failed to initialize RabbitMQ handler. Check your logs");
        exit(1);
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
