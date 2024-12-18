use chrono::{DateTime, NaiveDateTime};
use diesel::{prelude::*, result::Error};
use postgis_diesel::types::Point;
use roadsense_diesel::schema::bump_records;
use std::sync::Arc;

use crate::message::JsonMessage;

// Define the insertable struct for Diesel
#[derive(Debug, Insertable)]
#[diesel(table_name = bump_records)]
pub struct BumpRecordInsert {
    pub device_id: String,
    pub created_at: NaiveDateTime,
    pub bumpiness_factor: i16,
    pub location: Point,
}

// Function to process and insert a batch of records
pub async fn process_batch(
    conn: &mut PgConnection,
    records: &[Arc<JsonMessage>],
) -> Result<usize, Error> {
    // Map the JsonMessage vector into NewBumpRecord vector
    let new_records: Vec<BumpRecordInsert> = records
        .iter()
        .map(|record| BumpRecordInsert {
            device_id: record.device_id.clone(),
            created_at: DateTime::from_timestamp(record.timestamp, 0)
                .unwrap()
                .naive_utc(),
            bumpiness_factor: record.bumpiness,
            location: Point {
                x: record.lon,
                y: record.lat,
                srid: Some(4326),
            },
        })
        .collect();

    // Use OSRM to snap the location to the nearest road
    let new_records = crate::osrm::snap_to_road(new_records).await;

    // Perform the batch insert
    diesel::insert_into(bump_records::table)
        .values(&new_records)
        .execute(conn)
}
