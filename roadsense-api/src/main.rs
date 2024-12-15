mod db;

use std::sync::{Arc, Mutex};

use actix_web::{web::Data, App, HttpServer};
use dotenv::dotenv;
use log::{error, info};

struct AppState {
    db: Arc<Mutex<diesel::PgConnection>>,
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    // Load the environment variables
    dotenv().ok();

    // initialize the env logger
    env_logger::init();

    // establish connection to the database
    info!("Connecting to database...");
    let conn = db::establish_connection();
    info!("Connected to database successfully.");

    // Create app data with the connection
    let app_data = Data::new(AppState {
        db: Arc::new(Mutex::new(conn)),
    });

    // extract host and port from the environment variables
    let host = std::env::var("HOST").unwrap_or_else(|_| {
        error!("HOST not found in environment variables. Using default value");
        "localhost".into() // default host
    });
    let port: u16 = std::env::var("PORT")
        .unwrap_or_else(|_| {
            error!("PORT not found in environment variables. Using default value");
            "8080".into() // default port
        })
        .parse()
        .expect("PORT must be an integer");

    // Data validation
    if !(1..=65535).contains(&port) {
        error!("PORT must be between 1 and 65535");
        std::process::exit(1);
    }

    // Define and start HTTP server
    HttpServer::new(move || App::new().app_data(app_data.clone()))
        .bind((host.as_str(), port))?
        .run()
        .await
}
