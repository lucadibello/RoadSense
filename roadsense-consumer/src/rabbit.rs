use lapin::{Connection, ConnectionProperties};
use log::{error, info};
use std::env;
use std::error::Error;

pub struct Rabbit {
    connection: Connection,
}

impl Rabbit {
    pub fn new(connection: Connection) -> Self {
        Rabbit { connection }
    }

    // close connection
    pub async fn close(&self) {
        if let Err(e) = self.connection.close(0, "Bye").await {
            error!("Failed to close RabbitMQ connection: {}", e);
        }
    }

    pub async fn consume(&self) -> Result<(), Box<dyn Error>> {
        // FIXME: Work in progress

        // create a channel
        let channel = self.connection.create_channel().await?;
        // declare a queue
        let queue = channel
            .queue_declare("hello", Default::default(), Default::default())
            .await?;
        info!("Declared queue {:?}", queue);
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
    Ok(Rabbit::new(conn?))
}
