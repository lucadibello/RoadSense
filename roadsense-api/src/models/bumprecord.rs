use chrono::NaiveDateTime;
use diesel::prelude::*;
use diesel::{prelude::Queryable, PgConnection, Selectable};
use postgis_diesel::functions::st_intersects;
use postgis_diesel::types::{Point, Polygon};
use roadsense_diesel::schema::bump_records::dsl::*;
use serde::Serialize;

// Define the insertable struct for Diesel
#[derive(Debug, Queryable, Selectable, Serialize)]
#[diesel(table_name = roadsense_diesel::schema::bump_records)]
pub struct BumpRecord {
    pub device_id: String,
    #[serde(with = "native_datetime_serde")]
    pub created_at: NaiveDateTime,
    pub bumpiness_factor: i16,
    pub location: Point,
}

impl BumpRecord {
    pub fn _get_all(conn: &mut PgConnection) -> QueryResult<Vec<BumpRecord>> {
        bump_records
            .select(BumpRecord::as_select())
            .load::<BumpRecord>(conn)
    }

    pub fn get_all_in_bounds(
        conn: &mut PgConnection,
        north: f64,
        south: f64,
        east: f64,
        west: f64,
    ) -> QueryResult<Vec<BumpRecord>> {
        // Define the SRID for the location column (4326 for WGS84)
        const SRID: u32 = 4326;

        // Query the database for all bump records in the bounds
        bump_records
            .filter(st_intersects(
                // our location column
                roadsense_diesel::schema::bump_records::location,
                // The polygon representing the map bounds (closed loop)
                Polygon::new(Some(SRID))
                    .add_point(Point::new(east, north, Some(SRID)))
                    .add_point(Point::new(east, south, Some(SRID)))
                    .add_point(Point::new(west, south, Some(SRID)))
                    .add_point(Point::new(west, north, Some(SRID)))
                    .add_point(Point::new(east, north, Some(SRID))) // Close the polygon
                    .clone(),
            ))
            .select(BumpRecord::as_select())
            .load::<BumpRecord>(conn)
    }
}

// define module for chrono's NativeDateTime serialization/deserialization
mod native_datetime_serde {
    use chrono::{DateTime, NaiveDateTime};
    use serde::{self, Deserialize, Deserializer, Serializer};
    pub fn serialize<S>(date: &NaiveDateTime, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let timestamp = date.and_utc().timestamp();
        serializer.serialize_i64(timestamp)
    }

    pub fn _deserialize<'de, D>(deserializer: D) -> Result<NaiveDateTime, D::Error>
    where
        D: Deserializer<'de>,
    {
        let timestamp = i64::deserialize(deserializer)?;

        // create timestamp object
        match DateTime::from_timestamp(timestamp, 0) {
            Some(date) => Ok(date.naive_utc()),
            None => Err(serde::de::Error::custom("Invalid timestamp")),
        }
    }
}
