use lapin::message::Delivery;

const EXPECTED_CONTENT_TYPE: &str = "application/json";

pub fn validate_ampq_message(msg: &Delivery) -> bool {
    // parse the RabbitMQ message here
    let properties = &msg.properties;
    // validate whether the message is valid
    // extract content type from message
    let content_type = properties.content_type();

    let status = match content_type {
        Some(content_type) => {
            // ensure that content type is JSON
            content_type.as_str() == EXPECTED_CONTENT_TYPE
        }
        None => false,
    };

    status
}
