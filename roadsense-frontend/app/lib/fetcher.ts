import { BaseApiResponse } from "~/types/api";
import { ErrorWithStatus } from "./errors";

export const fetcher = async <T>(url: string, options: RequestInit = {}) => {
  console.log("fetcher", url, options);
  const res = await fetch(url, options);
  if (!res.ok) {
    // read body if present
    const body = await res.text();

    if (body) {
      throw new ErrorWithStatus(body, res.status);
    } else if (res.statusText) {
      throw new ErrorWithStatus(res.statusText, res.status);
    } else {
      throw new ErrorWithStatus("Unknown error", res.status);
    }
  }
  return res.json() as Promise<BaseApiResponse<T>>;
};
