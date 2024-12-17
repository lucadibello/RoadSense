import { useState } from "react";
import {
  MapContainer,
  TileLayer,
  Marker,
  Popup,
  useMapEvent,
} from "react-leaflet";
import type { LatLngTuple } from "leaflet";
import { RoadBump } from "~/types/services";
import { fetcher } from "~/lib/fetcher";

function FetchBumps({ setBumps }: { setBumps: (bumps: RoadBump[]) => void }) {
  useMapEvent("moveend", async (e) => {
    const bounds = e.target.getBounds();
    console.log(bounds);

    // Extract southwest and northeast coordinates
    const southWest = bounds.getSouthWest();
    const northEast = bounds.getNorthEast();

    console.log(southWest, northEast);

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
        {bumps.map((bump) => (
          <Marker
            key={bump.created_at}
            position={[bump.location.x, bump.location.y]}
          >
            <Popup>
              Bumpiness Factor: {bump.bumpiness_factor} <br />
              Device: {bump.device_id} <br />
              Time: {new Date(bump.created_at).toLocaleString()}
            </Popup>
          </Marker>
        ))}
      </MapContainer>
    </div>
  );
}
