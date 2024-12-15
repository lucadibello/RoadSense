use std::sync::Arc;

use crate::{message::JsonMessage, schema::bump_records};
use chrono::DateTime;
use diesel::{
    prelude::Insertable, result::Error, PgConnection, Queryable, RunQueryDsl, Selectable,
};

use super::geography::Geography;

// Define geometry type for PostGIS
#[derive(Queryable, Selectable)]
#[diesel(table_name = bump_records)]
#[diesel(check_for_backend(diesel::pg::Pg))]
struct BumpRecord {
    id: i32,
    device_id: String,
    created_at: chrono::NaiveDateTime,
    bumpiness_factor: i16,
    location: Geography,
}

// Define an insertable version of BumpRecord
#[derive(Insertable)]
#[diesel(table_name = bump_records)]
struct NewBumpRecord<'a> {
    device_id: &'a str,
    created_at: chrono::NaiveDateTime,
    bumpiness_factor: i16,
    location: Geography,
}

// Function to process and insert a batch of records
pub fn process_batch(
    conn: &mut PgConnection,
    records: &[Arc<JsonMessage>],
) -> Result<usize, Error> {
    // Map the JsonMessage vector into NewBumpRecord vector
    let new_records: Vec<NewBumpRecord> = records
        .iter()
        .map(|record| NewBumpRecord {
            device_id: &record.device_id,
            created_at: DateTime::from_timestamp(record.timestamp, 0)
                .unwrap()
                .naive_utc(),
            bumpiness_factor: record.bumpiness,
            location: Geography {
                lon: record.lon,
                lat: record.lat,
            },
        })
        .collect();

    // Perform the batch insert
    diesel::insert_into(bump_records::table)
        .values(&new_records)
        .execute(conn)
}
