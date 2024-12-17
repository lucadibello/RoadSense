use actix_web::{get, web, Responder, Result};
use serde::{Deserialize, Serialize};

use crate::{models::bumprecord::BumpRecord, AppState};

#[derive(Debug, Serialize)]
pub struct BumpApiResponse {
    pub data: Option<Vec<BumpRecord>>,
    pub message: Option<String>,
}

#[derive(Debug, Deserialize)]
struct Bounds {
    north: f64,
    south: f64,
    east: f64,
    west: f64,
}

#[get("/bumps")]
pub async fn get_bumps(
    app_state: web::Data<AppState>,
    query: web::Query<Bounds>,
) -> Result<impl Responder> {
    let mut connection = app_state.db.lock().map_err(|_| {
        actix_web::error::ErrorInternalServerError("Failed to get database connection")
    })?;

    // Query the database for all bump records
    let bumps_result = BumpRecord::get_all_in_bounds(
        &mut connection,
        query.north,
        query.south,
        query.east,
        query.west,
    );

    match bumps_result {
        Ok(bumps) => {
            // Build the response with the fetched bump records
            let response = BumpApiResponse {
                data: Some(bumps),
                message: None,
            };
            Ok(web::Json(response))
        }
        Err(err) => {
            // Handle database query errors
            eprintln!("Database error: {:?}", err);
            Err(actix_web::error::ErrorInternalServerError(
                "Failed to retrieve bump records",
            ))
        }
    }
}
