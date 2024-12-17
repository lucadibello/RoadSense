import { useState } from "react";
import {
  MapContainer,
  TileLayer,
  Marker,
  Popup,
  useMapEvent,
} from "react-leaflet";
import type { LatLngTuple } from "leaflet";
import L from "leaflet";
import { RoadBump } from "~/types/services";
import { fetcher } from "~/lib/fetcher";

// Helper to assign colors based on severity
// Helper to assign colors based on severity
const getMarkerColor = (bumpiness_factor: number) => {
  if (bumpiness_factor <= 50) return "#00FF00"; // Light Green
  if (bumpiness_factor <= 100) return "#ADFF2F"; // Yellow-Green
  if (bumpiness_factor <= 150) return "#FFFF00"; // Yellow
  if (bumpiness_factor <= 200) return "#FFA500"; // Orange
  return "#FF0000"; // Red
};

// Function to create a custom icon for markers
const createCustomIcon = (color: string) =>
  L.divIcon({
    html: `<div style="
            background-color: ${color};
            width: 20px;
            height: 20px;
            border-radius: 50%;
            border: 2px solid #000;
          "></div>`,
    className: "custom-icon",
  });

function FetchBumps({ setBumps }: { setBumps: (bumps: RoadBump[]) => void }) {
  useMapEvent("moveend", async (e) => {
    const bounds = e.target.getBounds();

    // Extract southwest and northeast coordinates
    const southWest = bounds.getSouthWest();
    const northEast = bounds.getNorthEast();

    // load bumps from the server
    const url = `${window.ENV.API_URL}/bumps?north=${northEast.lat}&south=${southWest.lat}&east=${northEast.lng}&west=${southWest.lng}`;
    const response = await fetcher<RoadBump[]>(url);
    const bumps = response.data;

    // set the bumps
    if (bumps) {
      setBumps(bumps);
    }
  });

  return null;
}

export default function Map() {
  const position: LatLngTuple = [46.0109711, 8.9606471];
  const [bumps, setBumps] = useState<RoadBump[]>([]);

  return (
    <div className="w-full h-full">
      <MapContainer
        style={{ height: "100%" }}
        center={position}
        zoom={13}
        scrollWheelZoom={true}
      >
        <TileLayer
          attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <FetchBumps setBumps={setBumps} />
        {bumps.map((bump, idx) => {
          const color = getMarkerColor(bump.bumpiness_factor);
          return (
            <Marker
              key={idx}
              position={[bump.location.y, bump.location.x]}
              icon={createCustomIcon(color)}
            >
              <Popup>
                Bumpiness Factor: {bump.bumpiness_factor} <br />
                Device: {bump.device_id} <br />
                Latitude: {bump.location.x} <br />
                Longitude: {bump.location.y} <br />
                Time: {new Date(bump.created_at).toLocaleString()}
              </Popup>
            </Marker>
          );
        })}
      </MapContainer>
    </div>
  );
}
