import type { MetaFunction } from "@remix-run/node";
import { Navbar } from "../components/ui/navbar"; // Adjust this path to where your Navbar is located

export const meta: MetaFunction = () => {
  return [
    { title: "RoadSense - Remix App" },
    { name: "description", content: "Welcome to RoadSense!" },
  ];
};

export default function Index() {
  return (
    <div className="h-screen flex flex-col">
      {/* Navbar */}
      <Navbar />

      {/* Main Content */}
      <div className="flex-1 flex items-center justify-center bg-background text-primary">
        <p>Hello hello!</p>
      </div>
    </div>
  );
}
