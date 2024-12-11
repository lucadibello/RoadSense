use std::env;

use lapin::message::Delivery;
use serde::Deserialize;

#[derive(Debug)]
pub struct QueueMessage {
    msg: Delivery,
    expected_content_type: String,
}

#[derive(Deserialize)]
pub struct JsonMessage {
    pub message: String,
}

impl QueueMessage {
    pub fn new(message: Delivery) -> Self {
        // load expected content type from env
        let expected_content_type =
            env::var("EXPECTED_CONTENT_TYPE").unwrap_or("application/json".to_string());

        QueueMessage {
            msg: message,
            expected_content_type,
        }
    }
}

pub trait MessageParser {
    fn parse_message(&self);
}

impl MessageParser for QueueMessage {
    fn parse_message(&self) {
        // extract content type from message
        let content_type = self.msg.properties.content_type();

        // ensure that content type is supported
        let status = match content_type {
            Some(content_type) => {
                // ensure that content type is JSON
                self.expected_content_type.eq(content_type.as_str())
            }
            None => false,
        };

        // Validate the message and proceed with decoding
        if status {
            // parse the message
            println!("Message is valid");
        } else {
            println!("Message is invalid");
        }
    }
}
