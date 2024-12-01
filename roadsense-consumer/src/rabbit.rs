use lapin::options::{BasicAckOptions, BasicConsumeOptions};
use lapin::{types::FieldTable, Connection, ConnectionProperties};

use futures_lite::stream::StreamExt;
// import required future traits

use log::{debug, error, info};
use std::env;
use std::error::Error;

pub struct Rabbit {
    connection: Connection,

    queue: String,
    tag: String,
    // exchange: String,
}

impl Rabbit {
    pub fn new(connection: Connection, queue: String, tag: String) -> Self {
        Rabbit {
            connection,
            queue,
            tag,
        }
    }

    // close connection
    pub async fn close(&self) {
        if let Err(e) = self.connection.close(0, "Bye").await {
            error!("Failed to close RabbitMQ connection: {}", e);
        }
    }

    pub async fn consume(&self) -> Result<(), Box<dyn Error>> {
        // create a channel
        debug!("Creating RabbitMQ channel...");
        let channel = self.connection.create_channel().await?;

        // Start consuming messages
        debug!("Defining consumer...");
        let mut consumer = channel
            .basic_consume(
                &self.queue,
                &self.tag,
                BasicConsumeOptions::default(),
                FieldTable::default(),
            )
            .await?;

        // Start consuming messages
        info!("Consuming messages...");
        while let Some(delivery) = consumer.next().await {
            println!("Received message");
            let delivery = delivery.expect("error in consumer");
            delivery.ack(BasicAckOptions::default()).await?;
        }

        // Return Ok if everything went well
        Ok(())
    }
}

pub async fn build() -> Result<Rabbit, Box<dyn Error>> {
    // Load env variables
    let user = env::var("RABBIT_USER").map_err(|e| format!("Missing RABBIT_USER: {}", e))?;
    let password =
        env::var("RABBIT_PASSWORD").map_err(|e| format!("Missing RABBIT_PASSWORD: {}", e))?;
    let host = env::var("RABBIT_HOST").map_err(|e| format!("Missing RABBIT_HOST: {}", e))?;
    let port = env::var("RABBIT_PORT").map_err(|e| format!("Missing RABBIT_PORT: {}", e))?;

    // Load queue and exchange
    let queue = env::var("RABBIT_QUEUE").map_err(|e| format!("Missing RABBIT_QUEUE: {}", e))?;
    // let exchange =
    //     env::var("RABBIT_EXCHANGE").map_err(|e| format!("Missing RABBIT_EXCHANGE: {}", e))?;
    let tag = env::var("CONSUMER_NAME").map_err(|e| format!("Missing CONSUMER_NAME: {}", e))?;

    // Build RabbitMQ address
    let addr = format!("amqp://{}:{}@{}:{}", user, password, host, port);

    // Log the connection attempt
    info!("Attempting to connect to RabbitMQ at {}", addr);

    // Connect to RabbitMQ
    let conn = match Connection::connect(&addr, ConnectionProperties::default()).await {
        Ok(conn) => {
            info!("Successfully connected to RabbitMQ");
            Ok(conn)
        }
        Err(e) => {
            error!("Failed to connect to RabbitMQ: {}", e);
            Err(Box::new(e))
        }
    };

    // return struct with connection
    Ok(Rabbit::new(conn?, queue, tag))
}
