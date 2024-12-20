import type { MetaFunction } from "@remix-run/node";
import { ClientOnly } from "remix-utils/client-only";
import Map from "../components/ui/map.client";
import { useState } from "react";
import { BumpFilter, severityFilters } from "~/util/bumpfilter";
import { BumpSeverity } from "~/types/bumps";
import { RoadBump } from "~/types/services";

export const meta: MetaFunction = () => {
  return [
    { title: "RoadSense - Healthier Roads, Safer Journeys" },
    { name: "description", content: "Welcome to RoadSense!" },
  ];
};

type AvailableFilter = { name: string; filter: BumpFilter; order: number };

// Filters mapped for user selection
const availableFilters: AvailableFilter[] = [
  {
    name: `Smooth (<= ${BumpSeverity.Smooth})`,
    filter: severityFilters.smooth,
    order: 0,
  },
  {
    name: `Minor (${BumpSeverity.Smooth + 1}-${BumpSeverity.Minor})`,
    filter: severityFilters.minor,
    order: 1,
  },
  {
    name: `Moderate (${BumpSeverity.Minor + 1}-${BumpSeverity.Moderate})`,
    filter: severityFilters.moderate,
    order: 2,
  },
  {
    name: `Major (${BumpSeverity.Moderate + 1}-${BumpSeverity.Major})`,
    filter: severityFilters.major,
    order: 3,
  },
  {
    name: `Severe (> ${BumpSeverity.Major})`,
    filter: severityFilters.severe,
    order: 4,
  },
];

export default function Index() {
  // State to hold selected filters as AvailableFilter[]
  const [selectedFilters, setSelectedFilters] = useState<AvailableFilter[]>([]);
  // State to save whether we need to show the heatmap of the single markings
  const [showHeatmap, setShowHeatmap] = useState<boolean>(false);

  // Toggle filter function
  const toggleFilter = (filter: AvailableFilter) => {
    setSelectedFilters((prevFilters) => {
      if (prevFilters.includes(filter)) {
        return prevFilters.filter((f) => f !== filter); // Remove filter
      } else {
        return [...prevFilters, filter]; // Add filter
      }
    });
  };

  // Prepare filters as a combined function
  const combinedFilters: BumpFilter | null =
    selectedFilters.length > 0
      ? (bumps: RoadBump[]): RoadBump[] =>
          selectedFilters
            .map((f) => f.filter(bumps)) // Apply all selected filters
            .flat() // Flatten the results
      : null;

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
            {() => (
              <Map
                filter={combinedFilters ? combinedFilters : undefined}
                showHeatmap={showHeatmap}
              />
            )}
          </ClientOnly>
        </div>

        {/* Options Panel */}
        <div className="w-1/4 bg-gray-100 p-4 border-l border-gray-300">
          <div className="text-center mb-4">
            <h2 className="text-xl font-bold">Filter Options</h2>
          </div>

          {/* Checkboxes for Filters */}
          <div>
            <p className="text-sm font-semibold">Severity Filters</p>
            {availableFilters.map((availableFilter) => (
              <div
                key={availableFilter.name}
                className="flex items-center mb-2"
              >
                <input
                  type="checkbox"
                  id={availableFilter.name}
                  onChange={() => toggleFilter(availableFilter)}
                  checked={selectedFilters.includes(availableFilter)}
                  className="mr-2"
                />
                <label
                  htmlFor={availableFilter.name}
                  className="text-sm cursor-pointer"
                >
                  {availableFilter.name}
                </label>
              </div>
            ))}
          </div>

          <div className="mt-4">
            <p className="text-sm font-semibold">Display Options</p>
            <div className="flex items-center">
              <input
                type="checkbox"
                id="showHeatmap"
                onChange={() => setShowHeatmap((prev) => !prev)}
                checked={showHeatmap}
                className="mr-2"
              />
              <label htmlFor="showHeatmap" className="text-sm cursor-pointer">
                Show Heatmap
              </label>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
