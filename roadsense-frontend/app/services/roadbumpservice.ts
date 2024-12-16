import { fetcher } from "~/lib/fetcher";
import { RoadBump } from "~/types/services";

if (!process.env.API_URL) {
  throw new Error("API_URL not found. Please define it in .env file");
}

const API_URL = process.env.API_URL;

export const getRoadBumps = async (): Promise<RoadBump[]> => {
  const response = await fetcher<RoadBump[]>(`${API_URL}/bumps`);
  return response.data;
};
