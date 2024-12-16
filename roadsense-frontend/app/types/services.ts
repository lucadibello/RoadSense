export interface RoadBump {
  device_id: string;
  created_at: string;
  bumpiness_factor: number;
  location: {
    latitude: number;
    longitude: number;
  };
}
