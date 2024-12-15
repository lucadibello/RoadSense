// @generated automatically by Diesel CLI.

diesel::table! {
    use diesel::sql_types::*;
    use postgis_diesel::sql_types::*;

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
