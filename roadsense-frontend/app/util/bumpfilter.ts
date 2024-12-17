import { BumpSeverity } from "~/types/bumps";
import { RoadBump } from "~/types/services";

export type BumpFilter = (bumps: RoadBump[]) => RoadBump[];

// Generate range-based filters dynamically
const createRangeFilter = (minSeverity: number, maxSeverity: number) => {
  return (bumps: RoadBump[]) =>
    bumps.filter(
      (bump) =>
        bump.bumpiness_factor > minSeverity &&
        bump.bumpiness_factor <= maxSeverity
    );
};

// Define severity filters
export const severityFilters = {
  smooth: (bumps: RoadBump[]) =>
    bumps.filter((bump) => bump.bumpiness_factor <= BumpSeverity.Smooth), // <= 50
  minor: createRangeFilter(BumpSeverity.Smooth, BumpSeverity.Minor), // 51–100
  moderate: createRangeFilter(BumpSeverity.Minor, BumpSeverity.Moderate), // 101–150
  major: createRangeFilter(BumpSeverity.Moderate, BumpSeverity.Major), // 151–200
  severe: (bumps: RoadBump[]) =>
    bumps.filter((bump) => bump.bumpiness_factor > BumpSeverity.Major), // > 200
};

// Function to apply selected filters sequentially
export const applyFilters = (
  bumps: RoadBump[],
  selectedFilters: BumpFilter[]
): RoadBump[] => {
  return selectedFilters.reduce(
    (filteredBumps, filterFn) => filterFn(filteredBumps),
    bumps
  );
};
