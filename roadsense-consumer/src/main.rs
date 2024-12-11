// import config.rs
mod config;
mod db;
mod message;
mod model;
mod rabbit;
mod schema;

use crate::config::load_env;
use tokio::{sync::mpsc, time::timeout};

use log::{debug, error, info};
use message::JsonMessage;
use std::{process::exit, sync::Arc, time::Duration};

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

    // connect to db
    info!("Connecting to database...");
    let _conn = db::establish_connection();
    info!("Connected to database successfully.");

    let res = rabbit::build().await;
    if res.is_err() {
        error!("Failed to initialize RabbitMQ handler. Check your logs");
        exit(1);
    }

    // get rabbit handler
    let rabbit = res.unwrap();

    // Create a channel to communicate with the RabbitMQ thread
    let (tx, mut rx) = mpsc::channel::<Arc<JsonMessage>>(1000);

    // Create new thread to consume messages from the queue
    let handle = tokio::spawn(async move {
        if let Err(e) = rabbit.consume(&tx).await {
            error!("Failed to consume messages. Error: {}", e);
        }
        // Close the rabbit connection
        if let Err(e) = rabbit.close().await {
            error!("Failed to close Rabbit connection. Error: {}", e);
        }
    });

    // Array to store bunch of messages
    const BATCH_SIZE: usize = 10;
    const TIMEOUT_SECS: u64 = 10;
    let mut batch = Vec::<Arc<JsonMessage>>::with_capacity(BATCH_SIZE);

    loop {
        // Wait for a message or timeout
        let result = timeout(Duration::from_secs(TIMEOUT_SECS), rx.recv()).await;

        match result {
            Ok(Some(msg)) => {
                // Add the received message to the batch
                batch.push(msg);
                info!(
                    "Received message. Batch size: {}/{}",
                    batch.len(),
                    BATCH_SIZE
                );

                // If the batch is full, process it
                if batch.len() >= BATCH_SIZE {
                    info!("Batch is full. Processing...");
                    // model::bumprecord::process_batch(&batch).await;
                    batch.clear();
                }
            }
            Ok(None) => {
                // The sender has closed the channel
                info!("Channel closed. Exiting.");
                break;
            }
            Err(_) => {
                // Timeout occurred
                if !batch.is_empty() {
                    info!("Timeout reached. Processing partial batch...");
                    // model::bumprecord::process_batch(&batch).await;
                    batch.clear();
                }
            }
        }
    }

    // Wait for the thread to finish
    if let Err(e) = handle.await {
        error!("Failed to join the thread. Error: {}", e);
    }
}
