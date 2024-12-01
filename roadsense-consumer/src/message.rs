use lapin::message::Delivery;

use crate::validation::validate_ampq_message;

pub struct QueueMessage {
    message: Delivery,
}

impl QueueMessage {
    pub fn new(message: Delivery) -> Self {
        QueueMessage { message }
    }
}

pub trait MessageParser {
    fn parse_message(&self);
}

impl MessageParser for QueueMessage {
    fn parse_message(&self) {
        // Validate the message and proceed with decoding
        if validate_ampq_message(&self.message) {
            // parse the message
            println!("Message is valid");
        } else {
            println!("Message is invalid");
        }
    }
}
