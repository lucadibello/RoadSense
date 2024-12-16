use chrono::NaiveDateTime;
use diesel::prelude::*;
use diesel::{prelude::Queryable, PgConnection, Selectable};
use postgis_diesel::types::Point;
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
    pub fn get_all(conn: &mut PgConnection) -> QueryResult<Vec<BumpRecord>> {
        bump_records
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
