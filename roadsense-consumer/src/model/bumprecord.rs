use crate::schema::bump_records;
use diesel::{data_types::PgTimestamp, Queryable, Selectable};

use super::geography::Geography;

// Define geometry type for PostGIS
#[derive(Queryable, Selectable)]
#[diesel(table_name = bump_records)]
#[diesel(check_for_backend(diesel::pg::Pg))]
struct BumpRecord {
    id: i32,
    device_id: String,
    created_at: PgTimestamp,
    bumpiness_factor: i16,
    location: Geography,
}
