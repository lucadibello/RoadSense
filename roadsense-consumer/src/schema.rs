// @generated automatically by Diesel CLI.

pub mod sql_types {
    #[derive(diesel::query_builder::QueryId, Clone, diesel::sql_types::SqlType)]
    #[diesel(postgres_type(name = "geography"))]
    pub struct Geography;
}

diesel::table! {
    use diesel::sql_types::*;
    use super::sql_types::Geography;

    bump_records (id, created_at) {
        id -> Int4,
        device_id -> Text,
        created_at -> Timestamptz,
        bumpiness_factor -> Int2,
        location -> Geography,
    }
}

diesel::table! {
    use diesel::sql_types::*;

    spatial_ref_sys (srid) {
        srid -> Int4,
        #[max_length = 256]
        auth_name -> Nullable<Varchar>,
        auth_srid -> Nullable<Int4>,
        #[max_length = 2048]
        srtext -> Nullable<Varchar>,
        #[max_length = 2048]
        proj4text -> Nullable<Varchar>,
    }
}

diesel::allow_tables_to_appear_in_same_query!(bump_records, spatial_ref_sys,);
