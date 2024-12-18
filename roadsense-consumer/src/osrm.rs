use crate::model::bumprecord::BumpRecordInsert;
use log::{error, info, warn};
use postgis_diesel::types::Point;
use reqwest::Client;
use serde::Deserialize;

// Root response
#[derive(Debug, Deserialize)]
pub struct OSRMResponse {
    pub code: String,
    pub matchings: Vec<Matching>,
    pub tracepoints: Option<Vec<Tracepoint>>,
}

#[derive(Debug, Deserialize)]
pub struct OSRMErrorResponse {
    pub code: String,
    pub message: String,
}

// Matching block
#[derive(Debug, Deserialize)]
pub struct Matching {
    pub confidence: f64,
    pub geometry: Geometry,
    pub legs: Vec<Leg>,
    pub weight_name: String,
    pub weight: f64,
    pub duration: f64,
    pub distance: f64,
}

// Geometry (LineString)
#[derive(Debug, Deserialize)]
pub struct Geometry {
    #[serde(rename = "type")]
    pub geo_type: String,
    pub coordinates: Vec<[f64; 2]>, // Longitude, Latitude
}

// Legs
#[derive(Debug, Deserialize)]
pub struct Leg {
    pub summary: String,
    pub weight: f64,
    pub duration: f64,
    pub distance: f64,
    pub steps: Vec<Step>, // Placeholder for steps if needed
}

// Steps (can be an empty array in this case)
#[derive(Debug, Deserialize)]
pub struct Step {}

// Tracepoint
#[derive(Debug, Deserialize)]
pub struct Tracepoint {
    pub waypoint_index: usize,
    pub matchings_index: usize,
    pub alternatives_count: usize,
    pub hint: String,
    pub distance: f64,
    pub name: String,
    pub location: [f64; 2], // Longitude, Latitude
}

pub async fn snap_to_road(original_points: Vec<BumpRecordInsert>) -> Vec<BumpRecordInsert> {
    // create HTTP client
    let client = Client::new();

    // Build the coordinate string: "x1,y1;x2,y2;..."
    let coordinates: Vec<String> = original_points
        .iter()
        .map(|p| format!("{},{}", p.location.x, p.location.y))
        .collect();
    let coordinate_string = coordinates.join(";");

    // Build the full URL with all points
    let url = format!(
        "http://router.project-osrm.org/match/v1/driving/{}?geometries=geojson",
        coordinate_string
    );

    // Send a single request to OSRM server
    let response = client
        .get(url)
        .send()
        .await
        .expect("Failed to send request");

    // If response not valid, print error and return original points
    if !response.status().is_success() {
        // Parse the error body as json
        let error_body = response.json::<OSRMErrorResponse>().await;
        if error_body.is_ok() {
            let error = error_body.unwrap();
            error!(
                "Street matching error: {} (code={}). Skipping snapping, returning original points",
                error.message, error.code
            );
            return original_points;
        }
        return original_points;
    }

    // Parse the response body as json
    let match_response = response.json::<OSRMResponse>().await;

    // If parsing failed, print error and return original points
    if match_response.is_err() {
        let error = match_response.err().unwrap();
        error!("Failed to parse response body: {}", error);
        return original_points;
    }
    let match_response = match_response.unwrap();

    // Otherwise, return snapped points
    let mut snapped_points = Vec::new();
    if let Some(tracepoints) = match_response.tracepoints {
        for (i, tracepoint) in tracepoints.iter().enumerate() {
            // If a valid tracepoint exists, use it
            let snapped_location = Point {
                x: tracepoint.location[0], // Longitude
                y: tracepoint.location[1], // Latitude
                srid: original_points[i].location.srid,
            };

            // Build the updated BumpRecordInsert
            snapped_points.push(BumpRecordInsert {
                device_id: original_points[i].device_id.clone(),
                created_at: original_points[i].created_at,
                bumpiness_factor: original_points[i].bumpiness_factor,
                location: snapped_location,
            });
        }

        info!(
            "Snapped points succesfully! # original points = {} -> # snapped points = {}",
            original_points.len(),
            snapped_points.len()
        );
    } else {
        // If no tracepoints are returned, retain all original points
        snapped_points = original_points;

        // Print a warning message that no tracepoints were returned
        warn!("No tracepoints returned. Retaining original points.");
    }

    snapped_points
}
