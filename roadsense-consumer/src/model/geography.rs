use diesel::deserialize::{self, FromSql};
use diesel::pg::{Pg, PgValue};
use diesel::serialize::{self, IsNull, Output, ToSql};
use diesel::sql_types::SqlType;
use std::io::Write;

// Define the `Geography` SQL type
#[derive(Debug, Clone, diesel::AsExpression, diesel::FromSqlRow)]
#[diesel(sql_type = crate::schema::sql_types::Geography)]
pub struct Geography {
    pub lon: f64,
    pub lat: f64,
}

// Implement FromSql for deserialization
impl FromSql<crate::schema::sql_types::Geography, Pg> for Geography {
    fn from_sql(value: PgValue<'_>) -> deserialize::Result<Self> {
        let geom_str = std::str::from_utf8(value.as_bytes())?;
        // Parse WKT format: POINT(lon lat)
        if let Some(point) = geom_str
            .strip_prefix("POINT(")
            .and_then(|s| s.strip_suffix(')'))
        {
            let coords: Vec<&str> = point.split_whitespace().collect();
            if coords.len() == 2 {
                let lon = coords[0].parse::<f64>()?;
                let lat = coords[1].parse::<f64>()?;
                return Ok(Geography { lon, lat });
            }
        }
        Err("Invalid geography format".into())
    }
}

// Implement ToSql for serialization
impl ToSql<crate::schema::sql_types::Geography, Pg> for Geography {
    fn to_sql<'b>(&'b self, out: &mut Output<'b, '_, Pg>) -> serialize::Result {
        let wkt = format!("POINT({} {})", self.lon, self.lat);
        out.write_all(wkt.as_bytes())?;
        Ok(IsNull::No)
    }
}
