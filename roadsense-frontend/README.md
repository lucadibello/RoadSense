# RoadSense Web Application

This is the frontend application for the RoadSense system, designed to visualize and interact with road condition data collected from IoT devices. The application displays real-time road quality data on an interactive map, including a heatmap and individual road quality points. It allows users to filter data by severity and view localized road conditions efficiently.

The frontend is built with [Remix](https://remix.run/), a modern full-stack web framework, and styled using [Tailwind CSS](https://tailwindcss.com/). It integrates map visualizations through [React Leaflet](https://react-leaflet.js.org/) and supports dynamic rendering of heatmaps using [leaflet.heat](https://github.com/Leaflet/Leaflet.heat).

## Features

- Interactive map for road condition visualization.
- Heatmap and point-based views for assessing road quality.
- Filter options for severity levels.
- Real-time data retrieval optimized for the visible map area.

---

## Environment Variables

The application uses an `.env` file to configure essential variables. To set up your environment:

1. Copy the example file provided in the project:

   ```sh
   cp .env.example .env
   ```

2. Open the `.env` file and configure the necessary variables. For example:

   ```plaintext
   # API URL (example: http://localhost:3000/api/v1)
   API_URL=http://localhost:3000/api/v1
   ```

The `API_URL` variable specifies the base URL of the backend API the frontend communicates with. Ensure this is set to the correct address for your development or production environment.

---

## Development

Run the development server locally:

```sh
bun run dev
```

This starts the application in development mode with hot-reloading for a smooth development experience.

## Deployment

Prepare your app for production by building the necessary files:

```sh
bun run build
```

After building, you can run the application in production mode:

```sh
bun start
```

This will serve the pre-built app using the built-in Remix app server.

## Styling

This project uses [Tailwind CSS](https://tailwindcss.com/) for styling, offering flexibility and a simple default configuration. However, you can replace or extend this setup with your preferred CSS framework or solution. For more details on CSS handling in Remix, refer to the [
