# RoadSense - API

The **RoadSense API** is a web service that provides access to road condition data (bump records) stored in the database. This service enables the retrieval of road anomaly data within specific geographical bounds, allowing for efficient querying and data delivery to the frontend application.

---

## Features

- **Retrieve Bump Records**: Fetch road anomaly data within user-specified geographical bounds.
- **Robust Error Handling**: Handles database connection issues and query failures gracefully.
- **Built with Rust**: Leverages the [Actix Web Framework](https://actix.rs/) for efficient and scalable web services.

---

## Environment Variables

The API service uses a `.env` file to configure its environment. Below is an example of the required variables:

> **Database Configuration**  
> `DATABASE_URL=postgres://postgres:postgres@db.roadsense.dev:5432/roadsense`

> **Webserver Configuration**  
> `HOST=localhost`  
> `PORT=3000`

> **CORS Options**  
> `ALLOWED_ORIGINS=http://localhost:5173`

> **Logger Configuration**  
> `RUST_LOG=info`  
> `RUST_BACKTRACE=0`

Make sure the `.env` file is properly configured before running the API.

---

## Endpoint

### **`GET /bumps`**

Fetch bump records within specified geographical bounds.

- **Query Parameters**:

  - `north` (float): Northern latitude of the bounding box.
  - `south` (float): Southern latitude of the bounding box.
  - `east` (float): Eastern longitude of the bounding box.
  - `west` (float): Western longitude of the bounding box.

- **Response**:

  - **`200 OK`**: Returns a JSON response containing the list of bump records within the bounds.

    ```json
    {
      "data": [
        {
          "id": 1,
          "latitude": 46.2044,
          "longitude": 6.1432,
          "severity": 150,
          "timestamp": "2024-12-01T10:30:00Z"
        }
      ],
      "message": null
    }
    ```

  - **`500 Internal Server Error`**: Returned if there is a database connection or query failure.

- **Example Request**: GET /bumps?north=46.2&south=46.0&east=6.2&west=6.0

---

## Running the Service

1. Ensure the database is running and accessible.
2. Configure the `.env` file with the appropriate variables.
3. Start the API server using:

   ```bash
   cargo run --release
   ```

The server will start and listen on the configured `HOST` and `PORT`.

---

## Notes

- Ensure the `ALLOWED_ORIGINS` variable matches the frontend's origin for proper CORS configuration.
- Debugging and monitoring can be adjusted using the `RUST_LOG` and `RUST_BACKTRACE` variables.
- For additional customization or enhancements, refer to the [Actix Web Documentation](https://actix.rs/) and [Diesel ORM Documentation](https://diesel.rs/).
