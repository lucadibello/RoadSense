export interface RoadBump {
  device_id: string;
  created_at: string;
  bumpiness_factor: number;
  location: {
    x: number;
    y: number;
    srid: number; // Spatial Reference Identifier (SRID)
  };
}
