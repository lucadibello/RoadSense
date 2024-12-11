-- Load the PostGIS extension
CREATE EXTENSION IF NOT EXISTS postgis;

-- Create the bump_records table
CREATE TABLE bump_records (
    id SERIAL,
    device_id TEXT NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    bumpiness_factor SMALLINT NOT NULL CHECK (bumpiness_factor BETWEEN 0 AND 255),
    location GEOMETRY(POINT, 4326) NOT NULL,
    PRIMARY KEY (id, created_at)
);

-- Create a hypertable in TimescaleDB
SELECT create_hypertable('bump_records', 'created_at');

