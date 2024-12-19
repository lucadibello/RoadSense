# RoadSense - Data Consumer

The **RoadSense Data Consumer** is a microservice responsible for consuming raw road condition data from the message queue, performing preprocessing, and storing the processed data in the database. This service acts as the final step in preparing road data for analysis and visualization.

---

## Key Features

- **Consumer and Preprocessing**:

  - The consumer retrieves data from the `roadsense` queue in RabbitMQ.
  - Data is processed in two threads:
    - The first thread pulls data from the queue and pushes it into a circular buffer.
    - Once the buffer is full, the second thread performs batch processing, including map matching and saving the data to the database.
  - Preprocessing ensures data is cleaned, mapped, and ready for use.

- **Batch Processing**:
  - Data is processed in batches for efficiency, with the size of the batch depending on the queue size.
  - The service uses [OSRM](http://project-osrm.org/) for map matching before inserting the data into the database.

---

## Environment Variables

The service relies on a `.env` file to configure essential parameters. Below is an example of the expected environment variables:

```bash
# RabbitMQ Configuration
RABBIT_HOST=localhost
RABBIT_PORT=5672
RABBIT_USER=roadsense
RABBIT_PASSWORD=roadsense
RABBIT_QUEUE=roadsense
RABBIT_EXCHANGE=roadsense

# General Consumer Configuration
CONSUMER_NAME=roadsense
SUPPORTED_CONTENT_TYPE=application/json

# Database Configuration
DATABASE_URL=postgres://postgres:postgres@db.roadsense.dev:5432/roadsense

# Queue Settings
QUEUE_SIZE=100
QUEUE_TIMEOUT_SECS=5

# Logger Configuration
RUST_LOG=debug
RUST_BACKTRACE=0
```

Make sure to configure these values appropriately for your environment.

---

## Workflow

1. **Message Consumption**: Data is pulled from the RabbitMQ queue and temporarily stored in a circular buffer.
2. **Batch Preprocessing**:
   - The buffer size and timeout are configured using the `QUEUE_SIZE` and `QUEUE_TIMEOUT_SECS` environment variables.
   - Once the buffer is full, data is passed to the preprocessing stage.
3. **Map Matching**: The service uses [OSRM](http://project-osrm.org/) to perform map matching on the batch of data.
4. **Database Storage**: Preprocessed data is stored in the PostgreSQL database for use by other components of the RoadSense system.

---

## Notes

- The service requires RabbitMQ to be running and accessible using the credentials specified in the `.env` file.
- OSRM must be properly configured and accessible for map matching operations to function correctly.
- Logging can be adjusted using the `RUST_LOG` and `RUST_BACKTRACE` environment variables for debugging and monitoring purposes.

Refer to the [RabbitMQ Documentation](https://www.rabbitmq.com/) and [Diesel ORM Documentation](https://diesel.rs/) for further details on dependencies and configuration.
