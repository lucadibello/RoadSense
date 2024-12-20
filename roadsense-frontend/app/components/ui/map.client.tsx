import { useEffect, useState } from "react";
import {
  MapContainer,
  TileLayer,
  Marker,
  Popup,
  useMapEvent,
  useMap,
} from "react-leaflet";
import "leaflet.heat";
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
  return "#FF0000"; // Red - Severe
  // if (bumpiness_factor <= BumpSeverity.Severe)
  // return "#800080"; // Purple - Extreme (not defined as it should never happen...)
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
}

function FetchBumps({ setBumps }: FetchBumpsProps) {
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
      // setBumps(filter ? applyFilters(bumps, [filter]) : bumps);
      setBumps(bumps);
    }
  });

  return null;
}

interface HeatMapProps {
  bumps: RoadBump[];
}

function HeatMap({ bumps }: HeatMapProps) {
  const map = useMap();

  useEffect(() => {
    const heatMapData = bumps.map((bump) => [
      bump.location.y,
      bump.location.x,
      (bump.bumpiness_factor - BumpSeverity.Smooth) / BumpSeverity.Severe / 3,
    ]);

    const heatLayer = L.heatLayer(heatMapData).addTo(map);

    return () => {
      heatLayer.remove(); // Cleanup on component unmount or rerender
    };
  }, [bumps, map]);

  return null;
}

interface MapProps {
  filter?: BumpFilter;
  showHeatmap?: boolean; // New prop to toggle heatmap display
}

export default function Map({ filter, showHeatmap = false }: MapProps) {
  const position: LatLngTuple = [46.0109711, 8.9606471];
  const [bumps, setBumps] = useState<RoadBump[]>([]);
  const [filteredBumps, setFilteredBumps] = useState<RoadBump[]>(bumps);

  useEffect(() => {
    // If filter is available, apply it!
    if (!!bumps && !!filter) {
      setFilteredBumps(applyFilters(bumps, [filter]));
    }
  }, [bumps, filter]);

  return (
    <div className="w-full h-full">
      <MapContainer
        style={{ height: "100%" }}
        center={position}
        zoom={30}
        scrollWheelZoom={true}
      >
        <TileLayer
          attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <FetchBumps setBumps={setBumps} />

        {/* Conditional rendering of HeatMap or Markers */}
        {showHeatmap ? (
          <HeatMap bumps={filter ? filteredBumps : bumps} />
        ) : (
          (filter ? filteredBumps : bumps).map((bump, idx) => {
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
          })
        )}
      </MapContainer>
    </div>
  );
}
