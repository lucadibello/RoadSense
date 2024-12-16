import React from "react";
import { cn } from "../../lib/utils"; // Assuming `cn` is a utility class function provided by shadcn
import { Button } from "./button"; // Assuming you use shadcn button component

export interface NavbarProps {}

const Navbar: React.FC<NavbarProps> = () => {
  return (
    <nav className={cn("bg-primary-foreground text-primary shadow-md")}>
      <div className="container mx-auto flex items-center justify-between py-4 px-6">
        {/* Logo */}
        <div className="text-xl font-bold">
          <a
            href="/"
            className="hover:text-accent hover:text-blue-600 transition-colors ease-in-out duration-300"
          >
            RoadSense{" "}
          </a>
          <span className="text-sm">| Healthier Roads, Safer Journeys</span>
        </div>

        {/* Right Section: Login/Sign Up or Profile */}
        <div>
          <Button variant="outline" className="ml-4" asChild>
            <a href="https://github.com/lucadibello/RoadSense">
              See on GitHub 🚀
            </a>
          </Button>
        </div>
      </div>
    </nav>
  );
};

export { Navbar };
