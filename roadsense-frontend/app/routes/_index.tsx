import type { MetaFunction } from "@remix-run/node";
import { ClientOnly } from "remix-utils/client-only";
import Map from "../components/ui/map.client";

export const meta: MetaFunction = () => {
  return [
    { title: "RoadSense - Remix App" },
    { name: "description", content: "Welcome to RoadSense!" },
  ];
};

export default function Index() {
  return (
    <div
      className="flex flex-col h-full"
      style={{
        minHeight: "calc(100vh - 68px)",
      }}
    >
      {/* Content below the Navbar */}
      <div className="flex flex-1">
        {/* Map Section */}
        <div className="flex-1 relative">
          <ClientOnly fallback={<div>Loading map...</div>}>
            {() => <Map />}
          </ClientOnly>
        </div>

        {/* Options Panel */}
        <div className="w-1/4 bg-gray-100 p-4 border-l border-gray-300">
          <div className="text-center mb-4">
            <h2 className="text-xl font-bold">Options</h2>
          </div>
          {/* Example Options */}
          <div>
            <p className="text-sm">Here you can add controls like:</p>
            <ul className="list-disc list-inside text-sm">
              <li>Fetch range slider</li>
              <li>Enable/Disable heatmap</li>
              <li>Other configurations</li>
            </ul>
          </div>
        </div>
      </div>
    </div>
  );
}
