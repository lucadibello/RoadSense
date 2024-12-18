mod config;
mod db;
mod message;
mod model;
mod osrm;
mod rabbit;

use crate::config::load_env;
use model::bumprecord;
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
    let mut conn = db::establish_connection();
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

    // Load batch size and timeout from environment variables
    let batch_size = std::env::var("QUEUE_SIZE")
        .expect("BATCH_SIZE must be set")
        .parse::<usize>()
        .unwrap_or(50);
    let timeout_secs = std::env::var("QUEUE_TIMEOUT_SECS")
        .expect("TIMEOUT_SECS must be set")
        .parse::<u64>()
        .unwrap_or(10);

    // Array to store bunch of messages
    let mut batch = Vec::<Arc<JsonMessage>>::with_capacity(batch_size);

    // FIXME: Just for test, we will add some points to the queue
    loop {
        // Wait for a message or timeout
        let result = timeout(Duration::from_secs(timeout_secs), rx.recv()).await;

        match result {
            Ok(Some(msg)) => {
                // Add the received message to the batch
                batch.push(msg);
                debug!(
                    "Received message. Batch size: {}/{}",
                    batch.len(),
                    batch_size
                );

                // If the batch is full, process it
                if batch.len() >= batch_size {
                    info!("Batch is full. Processing...");
                    let result = bumprecord::process_batch(&mut conn, &batch).await;
                    match result {
                        Ok(s) => {
                            info!("Batch processed successfully: {} records inserted", s);
                        }
                        Err(e) => {
                            error!("Failed to process batch. Error: {}", e);
                        }
                    }
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
                    let result = bumprecord::process_batch(&mut conn, &batch).await;
                    match result {
                        Ok(s) => {
                            info!("Batch processed successfully: {} records inserted", s);
                        }
                        Err(e) => {
                            error!("Failed to process batch. Error: {}", e);
                        }
                    }
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
