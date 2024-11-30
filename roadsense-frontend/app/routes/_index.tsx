import type { MetaFunction } from "@remix-run/node";

import Map from "../components/ui/map.client";
import { ClientOnly } from "remix-utils/client-only";

export const meta: MetaFunction = () => {
  return [
    { title: "RoadSense - Remix App" },
    { name: "description", content: "Welcome to RoadSense!" },
  ];
};

export default function Index() {
  return (
    <div className="h-screen flex flex-col">
      {/* Main Content */}
      <div className="flex-1 flex items-center justify-center">
        <ClientOnly fallback={<div>Loading map...</div>}>
          {() => <Map />}
        </ClientOnly>
      </div>
    </div>
  );
}
