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
import { BumpSeverity } from "~/types/bumps";
import { applyFilters, BumpFilter } from "~/util/bumpfilter";

// Helper to assign colors based on severity
const getMarkerColor = (bumpiness_factor: number) => {
  if (bumpiness_factor <= BumpSeverity.Smooth) return "#00FF00"; // Light Green
  if (bumpiness_factor <= BumpSeverity.Minor) return "#ADFF2F"; // Yellow-Green
  if (bumpiness_factor <= BumpSeverity.Moderate) return "#FFFF00"; // Yellow
  if (bumpiness_factor <= BumpSeverity.Major) return "#FFA500"; // Orange
  if (bumpiness_factor <= BumpSeverity.Severe) return "#FF0000"; // Red - Severe
  return "#800080"; // Purple - Extreme (not defined as it should never happpen...)
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

interface FetchBumpsProps {
  setBumps: (bumps: RoadBump[]) => void;
  filters: BumpFilter[];
}

function FetchBumps({ setBumps, filters }: FetchBumpsProps) {
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
      // Apply filters to the bumps if needed, otherwise set the bumps as is
      setBumps(filters ? applyFilters(bumps, filters) : bumps);
    }
  });

  return null;
}

interface MapProps {
  filters?: BumpFilter[];
}

export default function Map({ filters = [] }: MapProps) {
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
        <FetchBumps setBumps={setBumps} filters={filters} />
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
