use lapin::options::{BasicAckOptions, BasicConsumeOptions};
use lapin::{types::FieldTable, Connection, ConnectionProperties};

use futures_lite::stream::StreamExt;

use crate::message::QueueMessage;

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
        debug!("queue name: {}, tag: {}", &self.queue, &self.tag);

        // declare the queue we want to consume from
        let result = channel
            .queue_declare(&self.queue, Default::default(), Default::default())
            .await?;
        info!("Declared queue: {:?}", result);

        // Start consuming messages
        let mut consumer = channel
            .basic_consume(
                &self.queue,
                &self.tag,
                BasicConsumeOptions::default(),
                FieldTable::default(),
            )
            .await?;

        info!("Declared queue");

        // Start consuming messages
        info!("Consuming messages...");
        while let Some(delivery) = consumer.next().await {
            println!("Received message");
            let delivery = delivery.expect("error in consumer");

            // Ack the message to remove it from the queue
            delivery.ack(BasicAckOptions::default()).await?;

            // We wrap the delivered message into a QueueMessage struct
            let _msg = QueueMessage::new(delivery);

            // TODO: Validate and parse the message to get its JSON contents.

            // _msg.parse_message();
            println!("Parsed message");
        }

        // Return Ok if everything went well
        Ok(())
    }
}

pub async fn build() -> Result<Rabbit, Box<dyn Error>> {
    // Load env variables
    let user = env::var("RABBIT_USER").unwrap();
    let password = env::var("RABBIT_PASSWORD").unwrap();
    let host = env::var("RABBIT_HOST").unwrap();
    let port = env::var("RABBIT_PORT").unwrap();

    // Load queue and exchange
    let queue = env::var("RABBIT_QUEUE").unwrap();
    let tag = env::var("CONSUMER_NAME").unwrap();

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
