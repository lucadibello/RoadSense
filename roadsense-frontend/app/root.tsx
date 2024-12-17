import {
  Links,
  Meta,
  Outlet,
  Scripts,
  ScrollRestoration,
  useLoaderData,
} from "@remix-run/react";
import type { LinksFunction } from "@remix-run/node";

import tailwind_style from "./tailwind.css?url";
import leaftlet_style from "./styles/leaflet.css?url";
import { Navbar } from "./components/ui/navbar";

interface LoaderData {
  ENV: {
    API_URL: string;
  };
}

export const loader = async () => {
  return {
    ENV: {
      API_URL: process.env.API_URL,
    },
  };
};

export const links: LinksFunction = () => [
  { rel: "preconnect", href: "https://fonts.googleapis.com" },
  {
    rel: "preconnect",
    href: "https://fonts.gstatic.com",
    crossOrigin: "anonymous",
  },
  {
    rel: "stylesheet",
    href: "https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap",
  },
  // Stylesheet for the Tailwind CSS framework
  { rel: "stylesheet", href: tailwind_style },
  // Stylesheet for the Leaflet map library
  { rel: "stylesheet", href: leaftlet_style },
];

export function Layout({ children }: { children: React.ReactNode }) {
  // Load environment variables form the loader
  const data = useLoaderData<LoaderData>();

  return (
    <html lang="en">
      <head>
        <meta charSet="utf-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <Meta />
        <Links />
      </head>
      <body className="h-screen font-sans text-primary bg-primary-background">
        <Navbar />
        <main>{children}</main>
        <ScrollRestoration />
        <Scripts />
        <script
          dangerouslySetInnerHTML={{
            __html: `window.ENV = ${JSON.stringify(data.ENV)}`,
          }}
        />
      </body>
    </html>
  );
}

export default function App() {
  return <Outlet />;
}
